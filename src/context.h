#pragma once

#include <memory>
#include <filesystem>

#include <sys_fs/provider.h>
#include <video/gpu.h>
namespace Plusnx {
    enum SystemPaths {
        BaseDirectoryFs
    };

    class Context {
    public:
        Context();
        [[nodiscard]] SysFs::SysPath GetSystemPath(SystemPaths tagged) const;

        std::shared_ptr<SysFs::Provider> provider;
        std::shared_ptr<Video::GPU> gpu;
    };
}