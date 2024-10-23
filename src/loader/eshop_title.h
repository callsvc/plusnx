#pragma once
#include <loader/app_loader.h>

#include <sys_fs/fsys/regular_file.h>
#include <sys_fs/nx/partition_filesystem.h>
namespace Plusnx::Loader {
    class EShopTitle final : public AppLoader {
    public:
        EShopTitle(const SysFs::FileBackingPtr& nsp);

        void Load(std::shared_ptr<Core::Context>& process) override;

        std::unique_ptr<SysFs::Nx::PartitionFilesystem> pfs;
    };
}
