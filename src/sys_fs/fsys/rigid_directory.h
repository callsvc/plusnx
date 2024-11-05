#pragma once

#include <sys_fs/fs_types.h>
namespace Plusnx::SysFs::FSys {
    class RigidDirectory final : public RoDirectoryBacking {
    public:
        RigidDirectory() = default;
        RigidDirectory(const SysPath& path, bool create = false);

        FileBackingPtr OpenFile(const SysPath& filename) override;
        std::vector<SysPath> ListAllFiles() const override;
    };
}