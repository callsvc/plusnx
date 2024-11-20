#pragma once

#include <security/keyring.h>
#include <sys_fs/nx/nca_core.h>
#include <sys_fs/nx/partition_filesystem.h>
namespace Plusnx::SysFs::Nx {
    struct NcaType {
        ContentType contentType;
        ContentMetaType metaType;
    };
    struct NcaTypeKey {
        auto operator ()(const NcaType& type) const {
            const auto keyFirst{static_cast<u32>(type.contentType)};
            const auto keySecond{static_cast<u32>(type.metaType)};

            return std::hash<u32>()(keyFirst) ^ std::hash<u32>()(keySecond) << 1;
        }
        auto operator()(const NcaType& first, const NcaType& second) const {
            return std::tie(first.contentType, first.metaType) < std::tie(second.contentType, second.metaType);
        }
    };

    class SubmissionPackage {
    public:
        SubmissionPackage(const std::shared_ptr<Security::Keyring>& _keys, const FileBackingPtr& nsp);

        void ImportTicket(const SysPath& filename) const;

        std::shared_ptr<PartitionFilesystem> cnmt;
        std::vector<std::shared_ptr<NcaCore>> GetAllNcas() const;
        std::vector<std::shared_ptr<NcaCore>> GetIndexedNcas(ContentType type, ContentMetaType metaType) const;
        u64 GetProgramTitleId() const;
    private:
        std::unique_ptr<PartitionFilesystem> pfs;
        std::list<std::shared_ptr<NcaCore>> contents;
        std::map<NcaType, std::list<std::shared_ptr<NcaCore>>, NcaTypeKey> indexedNca;

        const std::shared_ptr<Security::Keyring> keys;
    };
}