#pragma once

#include <sys_fs/fs_types.h>
namespace Plusnx::SysFs::Nx {
    class PartitionFilesystem final : RoDirectoryBacking {
    public:
        std::vector<SysPath> ListAllFiles() override;
    };
}