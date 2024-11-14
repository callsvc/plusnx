#include <loader/game_fs.h>

namespace Plusnx::Loader {
    GameFs::GameFs(const SysFs::FSys::RigidDirectory &directory) : AppLoader(AppType::GameFs) {
        assert(std::filesystem::is_directory(directory.path));

        [[maybe_unused]] auto files{directory.ListAllFiles()};
    }
    void GameFs::Load([[maybe_unused]] std::shared_ptr<Core::Context> &context) {}
    std::shared_ptr<SysFs::Nx::ReadOnlyFilesystem> GameFs::GetRomFs(bool isControl) const {
        return nullptr;
    }
}
