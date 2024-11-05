#pragma once
#include <loader/app_loader.h>

#include <security/keyring.h>
#include <sys_fs/fsys/regular_file.h>
#include <sys_fs/nx/partition_filesystem.h>
#include <sys_fs/nx/content_archive.h>
#include <sys_fs/nx/content_metadata.h>
namespace Plusnx::Loader {
    class EShopTitle final : public AppLoader {
    public:
        EShopTitle(const std::shared_ptr<Security::Keyring>& _keys, const SysFs::FileBackingPtr& nsp);

        bool ExtractFilesInto(const SysFs::SysPath& path) const override;
        void Load(std::shared_ptr<Core::Context>& process) override;

        std::unique_ptr<SysFs::Nx::PartitionFilesystem> pfs;
    private:
        void ImportTicket(const SysFs::SysPath& filename) const;
        void IndexNcaEntries(const std::optional<SysFs::Nx::NCA>& metadata) const;

        std::optional<SysFs::SysPath> ValidateAllFiles(const std::vector<SysFs::SysPath>& files) const;
        std::list<SysFs::Nx::NCA> contents;
        const std::shared_ptr<Security::Keyring> keys;
    };
}
