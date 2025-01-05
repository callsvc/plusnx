#include <ranges>
#include <boost/endian/conversion.hpp>

#include <security/checksum.h>
#include <security/signatures.h>
#include <sys_fs/ctr_backing.h>
#include <sys_fs/layered_fs.h>
#include <sys_fs/streamed_file.h>

#include <sys_fs/nx/content_metadata.h>
#include <sys_fs/nx/nca_core.h>
namespace Plusnx::SysFs::Nx {
    constexpr auto XtsMode{Security::OperationMode::XtsAes};
    constexpr auto SectorSize{0x200};

    NcaCore::NcaCore(const std::shared_ptr<Security::Keyring>& _keys, const FileBackingPtr& nca) : keys(_keys), backing(nca) {
        if (nca->Read(content) != sizeof(content))
            return;
        if (encrypted = !ValidateMagic(content.magic); encrypted) {
            Security::K256 headerKey{};
            keys->GetKey256(Security::Key256Type::HeaderKey, headerKey.data(), headerKey.size());

            cipher.emplace(headerKey.data(), headerKey.size(), XtsMode, true);

            cipher->Decrypt(content, counter, SectorSize);
            if (!ValidateMagic(content.magic))
                throw exception("Corrupted or invalid NCA");

            assert(content.size == nca->GetSize());
        }

        if (content.keyGenerationOld != 2)
            std::print("Unrecognized legacy key version {}\n", content.keyGenerationOld);

        if (!content.fixedGeneration) {
            // https://gbatemp.net/threads/nca-signatures-verification-failed-tinfoil-nsp.533433/
            Security::Signatures verifier(Security::SignatureOperationType::NcaHdrSignatureFixed);
            const auto& signature{content.fixedSignature};
            if (verified = verifier.Verify(reinterpret_cast<const u8*>(&content.magic), 0x200, signature.data(), signature.size()); !verified)
                std::print("NCA signature verification failed\n");
        }

        rights = !IsValueEmpty(content.rights);

        type = content.type;
        path = backing->path;

        CreateFilesystemEntries();

        for (const auto& [type, file] : GetBackingFiles()) {
            if (auto testType = file->GetBytes(16); !testType.empty()) {
                if (type == FsType::PartitionFs)
                    assert(std::memcmp(testType.data(), "PFS0", 4) == 0);
                else if (type == FsType::RomFs)
                    assert(std::memcmp(testType.data(), "P\0\0\0", 4) == 0);
            }
        }
    }

    bool NcaCore::ValidateMagic(const u32 magic) {
        static std::vector<u32> magics;
        if (magics.empty()) {
            magics.reserve(4);
            for (const auto& value : {"NCA3", "NCA2", "NCA1", "NCA0"}) {
                magics.emplace_back(ConstMagic<u32>(value));
            }
        }
        return ContainsValue(magics, magic);
    }

    void NcaCore::CreateFilesystemEntries() {
        Security::Checksum checksum;
        std::array<u8, 0x20> result;
        for (const auto& [index, entry] : std::ranges::views::enumerate(content.entries)) {
            if (IsValueEmpty(entry))
                continue;

            const auto& header{content.headers[index]};
            assert(header.magic == 2);
            checksum.Update(&header, sizeof(header));
            checksum.Finish(result);

            if (!IsEmpty(content.headersSum[index]) && !IsEqual(result, content.headersSum[index])) {
                throw exception("The entry in the NCA at index {} appears to be corrupted", index);
            }
            CreateBackingFile(entry, header);
        }
    }

    void NcaCore::CreateBackingFile(const FsEntry& entry, const FsHeader& header) {
        u64 offset{entry.startOffset * 0x200};
        u64 size{};

        if (header.hashType == HashType::HierarchicalSha256Hash) {
            assert(header.hash256.layerCount == 2);
            offset += header.hash256.regions.back().offset;
            size = header.hash256.regions.back().size;

        }
        if (header.hashType == HashType::HierarchicalIntegrityHash) {
            assert(header.hashIntegrity.magic == ConstMagic<u32>("IVFC"));
            assert(header.hashIntegrity.maxLayers == 7);

            offset += header.hashIntegrity.levels.back().offset;
            size = header.hashIntegrity.levels.back().size;
        }
        if (offset + size > entry.endOffset * 0x200) {
            assert(size < content.size);
        }

        auto EmplaceBacking = [&] (FileBackingPtr&& file) {
            if (header.type == FsType::RomFs) {
                romfsList.emplace_back(std::move(file));
            } else if (header.type == FsType::PartitionFs) {
                pfsList.emplace_back(std::move(file));
            }
        };

        std::array<u8, 0x10> ctr{};
        const auto generation{boost::endian::endian_reverse(header.generation)};
        const auto secure{boost::endian::endian_reverse(header.secureValue)};
        std::memcpy(&ctr[0], &secure, 4);
        std::memcpy(&ctr[4], &generation, 4);

        if (header.encryptionType == EncryptionType::AesCtr || header.encryptionType == EncryptionType::AesCtrEx)
            EmplaceBacking(std::make_shared<CtrBacking>(backing, rights ? GetTitleKey() : GetAreaKey(header.encryptionType), offset, size, ctr));
        else if (header.encryptionType == EncryptionType::None)
            EmplaceBacking(std::make_shared<FileLayered>(backing, backing->path, offset, size));
    }

    Security::IndexedKeyType GetAreaType(const KeyAreaIndex area) {
        if (area == KeyAreaIndex::Application)
            return Security::IndexedKeyType::KekAreaApplication;
        if (area == KeyAreaIndex::Ocean)
            return Security::IndexedKeyType::KekAreaOcean;
        if (area == KeyAreaIndex::System)
            return Security::IndexedKeyType::KekAreaSystem;
        return Security::IndexedKeyType::Invalid;
    }

    Security::K128 NcaCore::GetTitleKey() const {
        if (IsEmpty(content.rights))
            return {};

        Security::K128 titleKek{};
        keys->GetIndexedKey(Security::IndexedKeyType::KekTitle, GetKeyRevision(), titleKek.data(), titleKek.size());
        Security::K128 title{};

        keys->GetTitleKey(content.rights, title.data(), title.size());

        Security::CipherCast decrypt(titleKek.data(), titleKek.size(), Security::OperationMode::EcbAes, true);
        Security::K128 decrypted{};
        decrypt.Process(decrypted.data(), title.data(), title.size());
        return decrypted;
    }

    Security::K128 NcaCore::GetAreaKey(const EncryptionType encType) const {
        const auto keyRevision{GetKeyRevision()};
        std::optional<Security::CipherCast> decrypt;

        const Security::K128 encryptedKey = [&] {
            Security::K128 key{};
            if (!keys->GetIndexedKey(GetAreaType(content.areaIndex), keyRevision, key.data(), key.size()))
                throw exception("Key not found");

            u32 indexArea{};
            if (encType == EncryptionType::AesCtrEx ||
                encType == EncryptionType::AesCtr)
                indexArea = 2;
            decrypt.emplace(key.data(), key.size(), Security::OperationMode::EcbAes, true);

            return content.encryptedKeyArea[indexArea];
        }();

        Security::K128 decryptedKey{};
        decrypt->Process(decryptedKey.data(), encryptedKey.data(), encryptedKey.size());

        return decryptedKey;
    }

    u64 NcaCore::GetKeyRevision() const {
        // https://github.com/strato-emu/strato/blob/master/app/src/main/cpp/skyline/vfs/nca.cpp#L97
        const auto legacy{static_cast<u64>(content.keyGenerationOld)};
        auto generation{static_cast<u64>(content.generation)};
        generation = std::max(legacy, generation);
        return std::min(generation - 1, generation);
    }

    std::vector<FileBackingPtr> NcaCore::GetBackingFiles(const bool partition) const {
        std::vector<FileBackingPtr> result;

        if (partition) {
            result.reserve(pfsList.size());
            std::ranges::copy(pfsList, std::back_inserter(result));
        } else {
            result.reserve(romfsList.size());
            std::ranges::copy(romfsList, std::back_inserter(result));
        }
        return result;
    }
    std::vector<std::pair<FsType, FileBackingPtr>> NcaCore::GetBackingFiles() const {
        std::vector<std::pair<FsType, FileBackingPtr>> result;
        result.reserve(pfsList.size() + romfsList.size());
        for (const auto& partition : GetBackingFiles(true)) {
            result.emplace_back(FsType::PartitionFs, partition);
        }
        for (const auto& romfs : GetBackingFiles(false)) {
            result.emplace_back(FsType::RomFs, romfs);
        }
        return result;
    }


    bool NcaCore::VerifyNca(std::array<u8, 0x10>& expected, Security::Checksum& checksum, std::vector<u8>& buffer, u64& readSize) const {
        auto filename{backing->path};
        if (GetEntryFormat(filename) == ContainedFormat::Cnmt)
            return {};

        while (filename.has_extension())
            filename = filename.replace_extension();

        expected = HexTextToByteArray<16>(filename.string());
        if (IsEmpty(expected))
            return {};

        const auto stream{std::make_unique<StreamedFile>(backing, false)};
        if (!stream)
            throw exception("The current NCA does not have valid backing");
#if 1
        // Skipping files larger than 512MiB for now
        if (stream->GetSize() > 512 * 1024 * 1024)
            return {};
#endif

        while (auto remain{stream->RemainBytes()}) {
            if (remain > buffer.size())
                remain = buffer.size();
            const auto size{stream->Read(buffer.data(), remain)};

            readSize += size;
            checksum.Update(buffer.data(), size);
        }

        return true;
    }
}
