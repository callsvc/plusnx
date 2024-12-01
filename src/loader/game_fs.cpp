#include <loader/game_fs.h>
namespace Plusnx::Loader {
    GameFs::GameFs(const std::shared_ptr<SysFs::FSys::RigidDirectory>& directory) : AppLoader(AppType::GameFs), backing(directory) {
        assert(std::filesystem::is_directory(directory->path));

        auto files{directory->ListAllFiles()};

        try {
            auto result{HexTextToByteArray<8>(directory->GetPath())};
            std::ranges::reverse(result);
            std::memcpy(&titleId, result.data(), sizeof(titleId));
        } catch ([[maybe_unused]] std::exception& except) {}
    }
    void GameFs::RegenerateGfs() const {
        const auto files{backing->ListAllFiles()};
        auto metaGfs{SysFs::StreamedFile(backing->CreateFile("gamefs.txt"))};
        for (const auto& file : files) {
            metaGfs << std::format("{}\n", file.string());
        }
    }

    void GameFs::Load([[maybe_unused]] std::shared_ptr<Core::Context>& context) {}
}
