#pragma once

#include <memory>
#include <filesystem>

#include <sys_fs/provider.h>
#include <video/gpu.h>
#include <audio/speaker.h>

// Kernel-related types
#include <gsk/types/kprocess.h>
namespace Plusnx::Core {
    enum SystemPaths {
        BaseDirectoryFs
    };

    class Context {
    public:
        Context();
        [[nodiscard]] SysFs::SysPath GetSystemPath(SystemPaths tagged) const;

        std::shared_ptr<SysFs::Provider> provider;
        std::shared_ptr<Video::GPU> gpu;
        std::shared_ptr<Audio::Speaker> speaker;

        // The main kernel object, necessary to run applications,
        // must live as long as the Application() class is still alive
        std::weak_ptr<Gsk::Types::KProcess> process;
    };
}