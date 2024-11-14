#include <ranges>
#include <platform/hostx1.h>

#include <core/context.h>
namespace Plusnx::Core {
    Context::Context() :
        configs(std::make_shared<Configs>()),
        provider(std::make_shared<SysFs::Provider>()),
        gpu(std::make_shared<Video::TegraX1Gpu>()),
        host1x(std::make_shared<Platform::Host1x>()),
        sound(std::make_shared<Audio::Sound>()) {
    }

    SysFs::SysPath Context::GetSystemPath(const SystemPaths tagged) const {
        switch (tagged) {
            case BaseDirectoryFs:
                return provider->GetRoot();
            default:
                assert(0);
        }
    }

    bool Context::IsFromSystemPath(const SysFs::SysPath& path) const {
        const auto target{path.parent_path()};
        for (const auto& directory : std::ranges::views::values(provider->dirs)) {
            if (ContainsValue(directory, target))
                return true;
        }
        return {};
    }
}
