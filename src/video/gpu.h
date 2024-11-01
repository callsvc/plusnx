#pragma once

#include <memory>

#include <video/vk/app_instance.h>
#include <video/vk/graphics_device.h>
namespace Plusnx::Video {
    class GPU {
    public:
        GPU() = default;
        void Initialize(const Vk::VkSupport& support);
        std::shared_ptr<Vk::AppInstance> instance;
        std::shared_ptr<Vk::GraphicsDevice> device;
    };
}