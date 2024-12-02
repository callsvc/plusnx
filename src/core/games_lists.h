#pragma once

#include <sys_fs/fsys/rigid_directory.h>
namespace Plusnx::Core {
    class GamesLists {
    public:
        GamesLists(const SysFs::FSys::RigidDirectory& dir) {
            lists.emplace_back(dir);
        }
        std::vector<SysFs::SysPath> GetAllGames() const;
        std::optional<SysFs::SysPath> SortAGame(const std::vector<std::string>& sorting) const;

        std::optional<SysFs::SysPath> first;
        u64 count{};

        void Initialize();
    private:
        std::vector<SysFs::FSys::RigidDirectory> lists;
    };
}