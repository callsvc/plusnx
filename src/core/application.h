#pragma once
#include <memory>
#include <core/games_lists.h>
#include <sys_fs/assets.h>
#include <video/vk/api_types.h>

#include <generic_kernel/types/kprocess.h>
namespace Plusnx::Os {
    class NxSys;
};

namespace Plusnx::Core {
    class Context;

    class Application {
    public:
        Application();
        void Initialize(const Video::Vk::VkSupport& support);
        void LoadAGameByIndex(u64 index = 0) const;

        // Only selects a game from the collection (does not load it yet)
        void PickByName(const std::string& game);
        // Convert various file formats into a GameFS
        bool ExtractIntoGameFs();

        std::shared_ptr<Context> context;
        std::shared_ptr<SysFs::Assets> assets;

        std::shared_ptr<Os::NxSys> nos;

        std::shared_ptr<GenericKernel::Kernel> kernel;
    private:
        std::string declared;
        SysFs::SysPath chosen;

        std::unique_ptr<GamesLists> games;
    };
}
