#pragma once

#include <memory>

#include <loader/app_loader.h>
#include <sys_fs/fsys/regular_file.h>

#include <os/process_creator.h>
namespace Plusnx::Core {
    struct Context;
}

namespace Plusnx::Os {
    class ProcessCreator;
    class NxSys {
    public:
        NxSys(const std::shared_ptr<Core::Context>& ctx) : context(ctx) {}

        void LoadApplicationFile(const SysFs::SysPath& path);
        void CheckNpdmPresence() const;
        SysFs::FileBackingPtr backing;

        std::shared_ptr<Loader::AppLoader> application;
        std::shared_ptr<Core::Context> context;

    private:
        void PopulateProcessParameters(const SysFs::MetaProgram& npdm) const;

        std::optional<ProcessCreator> creator;
    };
}