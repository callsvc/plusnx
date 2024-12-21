#include <print>
#include <core/games_lists.h>

namespace Plusnx::Core {
    std::vector<SysFs::SysPath> GamesLists::GetAllGames() const {
        std::vector<SysFs::SysPath> files;
        std::vector<SysFs::SysPath> marked;
        marked.reserve(lists.size());

        for (const auto& directory : lists) {
            for (const auto& already : marked) {
                auto here{directory.GetPath()};
                auto there{already.string()};
                if (here.contains(there))
                    throw exception("Recursion detected, the user must be joking");
            }
            auto content{directory.ListAllFiles()};
            marked.emplace_back(directory.path);

            std::ranges::move(content, std::back_inserter(files));
        }

        return files;
    }

    std::optional<SysFs::SysPath> GamesLists::SortAGame(const std::vector<std::string>& sorting) const {
        const auto allGames{GetAllGames()};

        if (allGames.empty())
            return std::nullopt;
        for (const auto& extension : sorting) {
            for (const auto& game : allGames) {
                if (game.extension() == extension)
                    return game;
            }
        }
        return allGames.front();
    }
    void GamesLists::Initialize() {
        const auto collection{GetAllGames()};
        for (const auto& game : collection) {
            const auto& filename{game.filename()};
            std::print("New game named {} added to the list\n", filename.string());
        }
        count = collection.size();

#if 1
        // We are running some tests here, and the NRO type should be prioritized over other types
        const std::vector<std::string> order{".nro", ".nsp"};
#else
        const std::vector<std::string> order{".nsp"};
#endif
        first = SortAGame(order);
    }
}
