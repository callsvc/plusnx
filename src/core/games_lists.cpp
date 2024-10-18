#include <print>
#include <core/games_lists.h>

namespace Plusnx::Core {
    std::vector<SysFs::SysPath> GamesLists::GetAllGames() const {
        std::vector<SysFs::SysPath> files;
        std::vector<SysFs::SysPath> marked;
        marked.reserve(lists.size());

        for (const auto& directory : lists) {
            for (const auto& already : marked) {
                auto here{directory.path.string()};
                auto there{already.string()};
                if (here.contains(there))
                    throw std::runtime_error("Recursion detected, the user must be joking");
            }
            auto content{directory.ListAllFiles()};
            marked.emplace_back(directory.path);

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
        for (const auto& game : collection) {
            const auto& filename{game.filename()};
            std::print("New game named {} added to the list\n", filename.string());
        }
        count = collection.size();
        first = SortAGame();
    }
}
