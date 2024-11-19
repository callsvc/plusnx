#pragma once

#include <sys_fs/nx/nso_core.h>
namespace Plusnx::SysFs::Ext {
    class NsoModules {
    public:
        NsoModules() = default;
        void LoadProgramImage(u64 startAddr, const std::vector<FileBackingPtr>& files);

        std::map<u64, std::shared_ptr<Nx::NsoCore>> modules;
    };
}