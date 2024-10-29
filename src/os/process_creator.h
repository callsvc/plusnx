#pragma once

#include <sys_fs/nx/readonly_filesystem.h>
#include <os/sdk/control_property.h>
namespace Plusnx::Os {
    class NxSys;
    class ProcessCreator {
    public:
        ProcessCreator(NxSys& nos) : nxOs(nos) {}
        void Initialize();
        void DumpRomContent();

        std::optional<SysFs::Nx::ReadOnlyFilesystem> romfs;
        std::optional<Sdk::ControlProperty> nacp;

        std::string title;
        std::string publisher;
        std::string version;

        u64 programId{};
    private:
        NxSys& nxOs;
    };
}
