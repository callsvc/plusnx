#include <sys_fs/assets.h>

#include <core/context.h>
namespace Plusnx::SysFs {
    Assets::Assets(const std::shared_ptr<Core::Context>& context) : provider(context->provider) {
        const auto root{context->GetSystemPath(Core::BaseDirectoryFs)};

        temp = root / "temp";
        logs = root / "logs";
        games = root / "games";
        keys = root / "keys";
        nand = root / "nand";
        cache = root / "cache";

        CheckDir(temp);
        CheckDir(logs);
        CheckDir(games);
        CheckDir(keys);
        CheckDir(nand);
        CheckDir(cache);

        ResetDir(logs);
        ResetDir(temp);
    }

    void Assets::CheckDir(const SysPath& path) const {
        provider->RegisterSystemPath(RootId, path);
    }

    void Assets::ResetDir(FSys::RigidDirectory& directory) const {
        for (const auto& file : directory.ListAllFiles()) {
            provider->RemoveCachedFile(file);
            directory.UnlikeFile(file);
        }
    }
}
