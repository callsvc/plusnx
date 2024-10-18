#include <core/games_lists.h>

namespace Plusnx::Core {
    std::vector<SysFs::SysPath> GamesLists::GetAllGames() const {
        std::vector<SysFs::SysPath> files;

        for (const auto& directory : lists) {
            auto content{directory.ListAllFiles()};
            std::ranges::move(content, std::back_inserter(files));
        }

        return files;
    }

    std::optional<SysFs::SysPath> GamesLists::SortAGame() const {
        const auto everything{GetAllGames()};
        if (!everything.empty())
            return everything.front();
        return std::nullopt;
    }
    void GamesLists::Initialize() {
        const auto collection{GetAllGames()};
        count = collection.size();
        first = SortAGame();
    }
}
