#pragma once

#include <sys_fs/nx/readonly_filesystem.h>
#include <os/sdk/control_property.h>
namespace Plusnx::Os {
    class NxSys;
    class ProcessCreator {
    public:
        ProcessCreator(NxSys& nos) : nxOs(nos) {}
        void Initialize();
        void DumpRomContent() const;

        std::shared_ptr<SysFs::Nx::ReadOnlyFilesystem> romfs;
        std::optional<Sdk::ControlProperty> nacp;

        std::string title{"Application"};
        std::string publisher{"None"};
        std::string version{"0.0"};

        u64 programId{};
    private:
        NxSys& nxOs;
    };
}
