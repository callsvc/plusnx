#include <ranges>

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

            if (!IsEqual(result, content.headersSum[index]))
                throw Except("The entry in the NCA at index {} appears to be corrupted", index);
            CreateBackingFile(nca, entry, header);
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

        emplaceBacking.emplace(std::make_shared<CtrBacking>(nca, GetDecryptionTitleKey(header.encryptionType), offset, size));
    }

    Security::IndexedKeyType GetAreaType(const KeyAreaIndex area) {
        if (area == KeyAreaIndex::Application)
            return Security::IndexedKeyType::KekAreaApplication;
        return Security::IndexedKeyType::Invalid;
    }

    Security::K128 NCA::GetDecryptionTitleKey(const EncryptionType encType) const {
        if (!IsValueEmpty(content.rights))
            return content.rights;

        const u32 keyRevision{std::max(static_cast<u32>(content.generation) - 1, 0U)};
        const Security::K128 encryptedKey = [&] {
            Security::K128 key{};
            keys->GetIndexedKey(GetAreaType(content.areaIndex), keyRevision, key.data(), key.size());

            u32 indexArea{};
            if (encType == EncryptionType::AesCtr)
                indexArea = 2;
            return content.encryptedKeyArea[indexArea];
        }();

        Security::K128 title{};
        Security::K128 decryptedKey{};
        keys->GetIndexedKey(Security::IndexedKeyType::KekTitle, keyRevision, title.data(), title.size());

        Security::CipherCast ecbDecrypt(title.data(), title.size(), Security::OperationMode::EcbAes, true);
        ecbDecrypt.Process(decryptedKey.data(), encryptedKey.data(), encryptedKey.size());

        return decryptedKey;
    }

    std::pair<FsType, FileBackingPtr> NCA::GetBackingFile() {
        if (romfs)
            return std::make_pair(FsType::RomFs, *romfs);
        if (pfs)
            return std::make_pair(FsType::PartitionFs, *pfs);
        return {};
    }
}
