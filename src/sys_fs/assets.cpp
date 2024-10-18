#include <sys_fs/assets.h>

#include <core/context.h>
namespace Plusnx::SysFs {
    Assets::Assets(const std::shared_ptr<Core::Context>& context) : provider(context->provider) {
        const auto root{context->GetSystemPath(Core::BaseDirectoryFs)};

        logs = root / "logs";
        games = root / "games";
        keys = root / "keys";
        nand = root / "nand";
        cache = root / "cache";

        CheckDir(logs);
        CheckDir(games);
        CheckDir(keys);
        CheckDir(nand);
        CheckDir(cache);
    }

    void Assets::CheckDir(const SysPath& path) const {
        provider->RegisterSystemPath(path);
    }
}
