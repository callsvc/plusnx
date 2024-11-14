#pragma once
#include <sys_fs/nx/readonly_filesystem.h>

namespace Plusnx::SysFs::Nx {
    class FakeRomFs final : public ReadOnlyFilesystem {
    public:
        FakeRomFs();

        void AddFile(const FileBackingPtr& file);
    };
}