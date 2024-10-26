#pragma once

#include <sys_fs/nx/readonly_filesystem.h>

namespace Plusnx::Os {
    class NxSys;
    class ProcessCreator {
    public:
        ProcessCreator(NxSys& nos) : nxOs(nos) {}
        void Initialize();

        void DumpRomContent();
        std::optional<SysFs::Nx::ReadOnlyFilesystem> romFs;
    private:
        NxSys& nxOs;
    };
}