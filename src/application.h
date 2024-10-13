#pragma once

#include <memory>

#include <sys_fs/assets.h>
#include <video/vk/vulkan_types.h>
namespace Plusnx {
    class Context;

    class Application {
    public:
        Application();
        void Initialize(const Video::Vk::VkSupport& support) const;

        std::shared_ptr<Context> context;
        std::shared_ptr<SysFs::Assets> assets;
    };
}
