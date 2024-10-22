#pragma once

#include <memory>

#include <loader/app_loader.h>
#include <sys_fs/fsys/regular_file.h>
namespace Plusnx::Core {
    class Context;
}

namespace Plusnx::Os {
    class NxSys {
    public:
        NxSys(const std::shared_ptr<Core::Context>& ctx) : context(ctx) {}

        void LoadApplicationFile(const SysFs::SysPath& path);
        SysFs::RegularFilePtr backing;

        std::shared_ptr<Loader::AppLoader> application;
    private:
        std::shared_ptr<Core::Context> context;
    };
}