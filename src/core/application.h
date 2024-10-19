#pragma once
#include <memory>
#include <core/games_lists.h>
#include <sys_fs/assets.h>
#include <video/vk/vulkan_types.h>

#include <gsk/types/kprocess.h>
namespace Plusnx::Core {
    class Context;

    class Application {
    public:
        Application();
        void Initialize(const Video::Vk::VkSupport& support);

        std::shared_ptr<Context> context;
        std::shared_ptr<SysFs::Assets> assets;
        std::shared_ptr<Gsk::Types::KProcess> pent;

        std::shared_ptr<Gsk::Kernel> kernel;
    private:
        std::unique_ptr<GamesLists> games;
    };
}
