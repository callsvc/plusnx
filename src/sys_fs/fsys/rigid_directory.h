#pragma once

#include <sys_fs/fs_types.h>
namespace Plusnx::SysFs::FSys {
    class RigidDirectory final : public DirectoryBacking {
    public:
        RigidDirectory() : DirectoryBacking("Empty") {}
        RigidDirectory(const SysPath& path, bool create = false);

        FileBackingPtr OpenFile(const SysPath& filename, FileMode mode = FileMode::Read) override;
        std::vector<SysPath> ListAllFiles() const override;

        FileBackingPtr CreateFile(const SysPath& file) override;
        void UnlikeFile(const SysPath& file) override;
    };
}