#pragma once

#include <memory>
#include <filesystem>

#include <sys_fs/provider.h>
#include <graphics/gpu.h>
namespace Plusnx {
    enum SystemPaths {
        BaseDirectoryFs
    };

    class Context {
    public:
        Context();
        std::filesystem::path GetSystemPath(SystemPaths tagged);

        std::shared_ptr<SysFs::Provider> provider;
        std::shared_ptr<Graphics::GPU> gpu;
    private:
        std::filesystem::path rootDir;
    };
}