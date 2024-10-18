#pragma once

#include <memory>

#include <sys_fs/assets.h>
#include <video/vk/vulkan_types.h>

#include <kmo/types/kprocess.h>
namespace Plusnx::Core {
    class Context;

    class Application {
    public:
        Application();
        void Initialize(const Video::Vk::VkSupport& support);

        std::shared_ptr<Context> context;
        std::shared_ptr<SysFs::Assets> assets;
        std::shared_ptr<Kmo::Types::KProcess> entity;

        std::shared_ptr<Kmo::Kernel> kernel;
    };
}
