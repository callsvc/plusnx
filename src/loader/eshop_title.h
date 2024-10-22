#pragma once
#include <loader/app_loader.h>

#include <sys_fs/fsys/regular_file.h>
#include <sys_fs/nx/partition_filesystem.h>
namespace Plusnx::Loader {
    class EShopTitle : public AppLoader {
    public:
        EShopTitle(const SysFs::RegularFilePtr& nsp);

        std::unique_ptr<SysFs::Nx::PartitionFilesystem> pfs;
    };
}
