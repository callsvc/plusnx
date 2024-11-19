#pragma once
#include <sys_fs/nx/readonly_filesystem.h>

namespace Plusnx::SysFs::Ext {
    class FakeRomFs final : public Nx::ReadOnlyFilesystem {
    public:
        FakeRomFs();

        void AddFile(const FileBackingPtr& file);
    };
}