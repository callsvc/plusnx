#include <ranges>

#include <core/context.h>
#include <sys_fs/assets.h>
namespace Plusnx::SysFs {
    Assets::Assets(const std::shared_ptr<Core::Context>& context) : provider(context->provider) {
        const auto root{GetSystemPath(SystemPaths::BaseDirectoryFs)};

        temp = root / "temp";
        user = root / "user";
        logs = root / "logs";
        games = root / "games";
        keys = root / "keys";
        nand = root / "nand";
        cache = root / "cache";

        CheckDir(temp);
        CheckDir(user);
        CheckDir(logs);
        CheckDir(games);
        CheckDir(keys);
        CheckDir(nand);
        CheckDir(cache);

        ResetDir(logs);
        ResetDir(temp);
    }

    SysPath Assets::GetSystemPath(const SystemPaths tagged) const {
        switch (tagged) {
            case SystemPaths::BaseDirectoryFs:
                return provider->GetRoot();
            default:
                assert(0);
        }
    }

    bool Assets::IsFromSystemPath(const SysPath& path) const {
        const auto target{path.parent_path()};
        for (const auto& directory : std::ranges::views::values(provider->dirs)) {
            if (ContainsValue(directory, target))
                return true;
        }
        return {};
    }

    void Assets::CheckDir(const SysPath& path) const {
        provider->RegisterSystemPath(RootId, path);
    }

    void Assets::ResetDir(FSys::RigidDirectory& directory) const {
        for (const auto& file : directory.ListAllFiles()) {
            provider->RemoveCachedFile(file);
            directory.UnlinkFile(file);
        }
    }

    SysPath Assets::GetPlusnxFilename(const PlusnxDirectoryType type) const {
        if (type == PlusnxDirectoryType::ConfigFile)
            return user.path / "config.toml";
        if (type == PlusnxDirectoryType::ConfigFileBackup)
            return temp.path / "config.bkp.toml";

        if (type == PlusnxDirectoryType::Database)
            return user.path / "played.db";

        if (type == PlusnxDirectoryType::Telemetry)
            return temp.path / "telemetry.bin";

        return {};
    }
}
