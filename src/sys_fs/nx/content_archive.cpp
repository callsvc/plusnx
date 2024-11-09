#include <ranges>
#include <boost/endian/conversion.hpp>

#include <security/checksum.h>
#include <sys_fs/ctr_backing.h>
#include <sys_fs/nx/content_archive.h>
namespace Plusnx::SysFs::Nx {
    constexpr auto XtsMode{Security::OperationMode::XtsAes};
    constexpr auto SectorSize{0x200};

    NCA::NCA(const std::shared_ptr<Security::Keyring>& _keys, const FileBackingPtr& nca) : keys(_keys) {
        if (nca->Read(content) != sizeof(content))
            return;
        if (!ValidateMagic(content.magic)) {
            Security::K256 headerKey{};
            keys->GetKey256(Security::Key256Type::HeaderKey, headerKey.data(), headerKey.size());

            cipher.emplace(headerKey.data(), headerKey.size(), XtsMode, true);

            cipher->Decrypt(content, counter, SectorSize);
            if (!ValidateMagic(content.magic))
                throw Except("Corrupted or invalid NCA");

            assert(content.size == nca->GetSize());
        }
        assert(content.keyGenerationOld == 2);

        rights = !IsValueEmpty(content.rights);
        CreateFilesystemEntries(nca);
    }

    bool NCA::ValidateMagic(const u32 magic) {
        static std::vector<u32> magics;
        if (magics.empty()) {
            magics.reserve(4);
            for (const auto& value : {"NCA3", "NCA2", "NCA1", "NCA0"}) {
                magics.emplace_back(ConstMagic<u32>(value));
            }
        }
        return ContainsValue(magics, magic);
    }

    void NCA::CreateFilesystemEntries(const FileBackingPtr& nca) {
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
                throw Except("The entry in the NCA at index {} appears to be corrupted", index);
            }
            CreateBackingFile(nca, entry, header);
            if (const auto file{GetBackingFile().second}) {
                if (auto testType = file->GetBytes(16); !testType.empty()) {
                    if (header.type == FsType::PartitionFs)
                        assert(std::memcmp(testType.data(), "PFS0", 4) == 0);
                    else if (header.type == FsType::RomFs)
                        assert(std::memcmp(testType.data(), "PFS0", 4));
                }
            }
        }
    }

    void NCA::CreateBackingFile(const FileBackingPtr& nca, const FsEntry& entry, const FsHeader& header) {
        u64 offset{entry.startOffset * 0x200};
        u64 size{};

        if (header.hashType == HashType::HierarchicalSha256Hash) {
            assert(header.hash256.layerCount == 2);
            offset += header.hash256.regions.back().offset;
            size = header.hash256.regions.back().size;
        }
        if (header.hashType == HashType::HierarchicalIntegrityHash) {
            assert(header.hashIntegrity.magic == ConstMagic<u32>("IVFC"));
            offset += header.hashIntegrity.levels.back().logicalOffset;
            size = header.hashIntegrity.levels.back().hashDataSize;
        }

        auto& emplaceBacking = [&] -> std::optional<FileBackingPtr>& {
            if (header.type == FsType::RomFs)
                return romfs;
            assert(header.type == FsType::PartitionFs);
            return pfs;
        }();

        std::array<u8, 16> ctr{};
        const auto gen{boost::endian::endian_reverse(header.generation)};
        const auto secure{boost::endian::endian_reverse(header.secureValue)};
        std::memcpy(&ctr[0], &gen, 4);
        std::memcpy(&ctr[4], &secure, 4);

        emplaceBacking.emplace(std::make_shared<CtrBacking>(nca, GetDecryptionTitleKey(header.encryptionType), offset, size, ctr));
    }

    Security::IndexedKeyType GetAreaType(const KeyAreaIndex area) {
        if (area == KeyAreaIndex::Application)
            return Security::IndexedKeyType::KekAreaApplication;
        return Security::IndexedKeyType::Invalid;
    }

    Security::K128 NCA::GetDecryptionTitleKey(const EncryptionType encType) const {
        const auto keyRevision{GetKeyRevision()};

        if (rights) {
            Security::K128 titleKek{};
            keys->GetIndexedKey(Security::IndexedKeyType::KekTitle, keyRevision, titleKek.data(), titleKek.size());
            Security::CipherCast titleCipher(titleKek.data(), titleKek.size(), Security::OperationMode::EcbAes, true);

            Security::K128 title{};
            titleCipher.Process(title.data(), title.data(), sizeof(title));
            return title;
        }

        std::optional<Security::CipherCast> ecbDecrypt;
        const Security::K128 encryptedKey = [&] {
            Security::K128 key{};
            keys->GetIndexedKey(GetAreaType(content.areaIndex), keyRevision, key.data(), key.size());

            u32 indexArea{};
            if (encType == EncryptionType::AesCtrEx ||
                encType == EncryptionType::AesCtr)
                indexArea = 2;
            ecbDecrypt.emplace(key.data(), key.size(), Security::OperationMode::EcbAes, true);

            return content.encryptedKeyArea[indexArea];
        }();

        Security::K128 decryptedKey{};
        ecbDecrypt->Process(decryptedKey.data(), encryptedKey.data(), encryptedKey.size());

        return decryptedKey;
    }

    u64 NCA::GetKeyRevision() const {
        // https://github.com/strato-emu/strato/blob/master/app/src/main/cpp/skyline/vfs/nca.cpp#L97
        const auto legacy{static_cast<u64>(content.keyGenerationOld)};
        auto generation{static_cast<u64>(content.generation)};
        generation = std::max(legacy, generation);
        return std::max(generation - 1, 0UL);
    }

    std::pair<FsType, FileBackingPtr> NCA::GetBackingFile() {
        if (romfs)
            return std::make_pair(FsType::RomFs, *romfs);
        if (pfs)
            return std::make_pair(FsType::PartitionFs, *pfs);
        return {};
    }
}
