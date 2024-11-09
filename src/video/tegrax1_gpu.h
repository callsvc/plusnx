#pragma once

#include <memory>

#include <video/vk_render/app_instance.h>
#include <video/vk_render/graphics_device.h>
namespace Plusnx::Video {
    class TegraX1Gpu {
    public:
        TegraX1Gpu() = default;
        void Initialize(const std::shared_ptr<GraphicsSupportContext>& support);

        std::shared_ptr<VkRender::AppInstance> instance;
        std::shared_ptr<VkRender::GraphicsDevice> device;
    };
}