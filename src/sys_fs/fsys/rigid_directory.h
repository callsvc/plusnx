#pragma once

#include <sys_fs/fs_types.h>
namespace Plusnx::SysFs::Fsys {
    class RigidDirectory final : public RoDirectoryBacking {
    public:
        RigidDirectory() = default;
        RigidDirectory(const SysPath& path) : RoDirectoryBacking(path) {}

        FileBackingPtr OpenFile(const SysPath& path) const override;
        std::vector<SysPath> ListAllFiles() const override;
    };
}