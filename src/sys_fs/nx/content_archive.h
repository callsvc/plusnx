#pragma once
#include <sys_fs/fs_types.h>

namespace Plusnx::SysFs::Nx {
    class NCA {
    public:
        NCA(const FileBackingPtr& nca);

        const FileBackingPtr backing;
    };
}
