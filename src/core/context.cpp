#include <ranges>
#include <core/context.h>

namespace Plusnx::Core {
    Context::Context() :
        provider(std::make_shared<SysFs::Provider>()),
        gpu(std::make_shared<Video::GPU>()),
        speaker(std::make_shared<Audio::Speaker>()) {
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
