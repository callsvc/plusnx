#pragma once

#include <sys_fs/fs_types.h>
namespace Plusnx::SysFs::Fsys {
    class RigidDirectory final : public RoDirectoryBacking {
    public:
        RigidDirectory() = default;
        RigidDirectory(const SysPath& path) : RoDirectoryBacking(path) {}

        std::vector<SysPath> ListAllFiles() const override;
    };
}