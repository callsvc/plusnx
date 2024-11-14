#pragma once

#include <memory>
#include <filesystem>

#include <core/configs.h>
#include <sys_fs/provider.h>
#include <video/tegrax1_gpu.h>
#include <audio/sound.h>

#include <services/settings/languages.h>

// Kernel-related types
#include <generic_kernel/types/kprocess.h>

namespace Plusnx::Os {
    class NxSys;
}
namespace Plusnx::Security {
    class Keyring;
}
namespace Plusnx::Platform {
    class Host1x;
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

        std::shared_ptr<Configs> configs;
        std::shared_ptr<SysFs::Provider> provider;
        std::shared_ptr<Security::Keyring> keys;
        std::shared_ptr<Video::TegraX1Gpu> gpu;
        std::shared_ptr<Platform::Host1x> host1x; // <! Our communication bridge between the CPU and GPU

        std::shared_ptr<Audio::Sound> sound;

        // User-selected language
        Services::Settings::LanguageType language{};

        std::shared_ptr<Os::NxSys> nxOs;

        // The main kernel object, necessary to run applications,
        // must live as long as the Application() class is still alive
        std::shared_ptr<GenericKernel::Types::KProcess> process;
    };
}