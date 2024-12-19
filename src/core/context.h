#pragma once

#include <sys_fs/assets.h>
#include <sys_fs/provider.h>
#include <core/configs.h>
#include <video/tegrax1_gpu.h>
#include <audio/sound.h>

#include <services/settings/languages.h>

// Kernel-related types
#include <nxk/types/kprocess.h>

#include <core/process_qol.h>
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

    struct Context : std::enable_shared_from_this<Context> {
        Context();

        void Initialize(const std::shared_ptr<Context>& context);
        void Destroy();

        std::shared_ptr<SysFs::Assets> assets;
        std::shared_ptr<Configs> configs;
        std::shared_ptr<SysFs::Provider> provider;
        std::shared_ptr<Security::Keyring> keys;
        std::shared_ptr<Video::TegraX1Gpu> gpu;
        std::shared_ptr<Platform::Host1x> host1x; // <! Our communication bridge between the CPU and GPU

        std::shared_ptr<Audio::Sound> sound;

        // User-selected language
        Services::Settings::LanguageType language{};
        std::optional<GameInformation> details;

        std::shared_ptr<Nxk::Kernel> kernel;
        std::shared_ptr<Os::NxSys> nxOs;

        // The main kernel object, necessary to run applications,
        // must live as long as the Application() class is still alive
        std::shared_ptr<Nxk::Types::KProcess> process;
    };
}