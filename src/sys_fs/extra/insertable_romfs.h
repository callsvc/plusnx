#pragma once
#include <sys_fs/nx/readonly_filesystem.h>

namespace Plusnx::SysFs::Extra {
    class InsertableRomFs final : public Nx::ReadOnlyFilesystem {
    public:
        InsertableRomFs();

        void AddFile(const FileBackingPtr& file);
    };
}