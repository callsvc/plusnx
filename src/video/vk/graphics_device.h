#pragma once

#include <video/vk/app_instance.h>
#include <types.h>
namespace Plusnx::Video::Vk {
    class GraphicsDevice {
    public:
        GraphicsDevice(const std::shared_ptr<AppInstance>& app);
        vk::Instance vkInstance;
    };
}