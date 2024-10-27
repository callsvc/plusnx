#pragma once

#include <memory>
#include <filesystem>

#include <sys_fs/provider.h>
#include <video/gpu.h>
#include <audio/speaker.h>

#include <services/settings/languages.h>

// Kernel-related types
#include <gsk/types/kprocess.h>
namespace Plusnx::Os {
    class NxSys;
}
namespace Plusnx::Core {
    enum SystemPaths {
        BaseDirectoryFs
    };

    class Context {
    public:
        Context();
        [[nodiscard]] SysFs::SysPath GetSystemPath(SystemPaths tagged) const;
        bool IsFromSystemPath(const SysFs::SysPath& path) const;

        std::shared_ptr<SysFs::Provider> provider;
        std::shared_ptr<Video::GPU> gpu;
        std::shared_ptr<Audio::Speaker> speaker;

        // User-selected language
        Services::Settings::LanguageType language;

        std::weak_ptr<Os::NxSys> nxOs;

        // The main kernel object, necessary to run applications,
        // must live as long as the Application() class is still alive
        std::weak_ptr<Gsk::Types::KProcess> process;
    };
}