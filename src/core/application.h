#pragma once
#include <memory>
#include <core/games_lists.h>
#include <sys_fs/assets.h>
#include <video/vk/vulkan_types.h>

#include <gsk/types/kprocess.h>
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

        std::shared_ptr<Context> context;
        std::shared_ptr<SysFs::Assets> assets;

        std::shared_ptr<Os::NxSys> nos;

        std::shared_ptr<Gsk::Kernel> kernel;
    private:
        std::unique_ptr<GamesLists> games;
    };
}
