#pragma once
#include <sys_fs/fs_types.h>

namespace Plusnx::SysFs::Nx {
    class CNMT {
    public:
        explicit CNMT(const FileBackingPtr& cnmt);
    };
}
