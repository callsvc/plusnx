#pragma once

#include <types.h>
#include <video/vk_render/app_instance.h>
namespace Plusnx::Video::VkRender {
    class GraphicsDevice {
    public:
        GraphicsDevice(const std::shared_ptr<AppInstance>& app);
        vk::Instance instance;
    };
}