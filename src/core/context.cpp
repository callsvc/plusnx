#include <platform/hostx1.h>
#include <core/context.h>

namespace Plusnx::Core {
    using DirectoryType = SysFs::PlusnxDirectoryType;

    Context::Context() :
        configs(std::make_shared<Configs>()),
        provider(std::make_shared<SysFs::Provider>()),
        gpu(std::make_shared<Video::TegraX1Gpu>()),
        host1x(std::make_shared<Platform::Host1x>()),
        sound(std::make_shared<Audio::Sound>()) {

    }

    void Context::Initialize(const std::shared_ptr<Context>& context) {
        assets = std::make_shared<SysFs::Assets>(context);

        configs->Initialize(assets->GetPlusnxFilename(DirectoryType::ConfigFile));
        configs->ExportConfigs(assets->GetPlusnxFilename(DirectoryType::ConfigFileBackup));
    }

    void Context::Destroy() {
        {
            auto ctxNxOs{std::move(nxOs)};
            process.reset();
            auto ctxKernel{std::move(kernel)};
        }
    }
}
