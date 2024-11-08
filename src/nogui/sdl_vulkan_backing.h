#pragma once
#include <vector>

#include <nogui/sdl_window.h>

#include <video/graphics_types.h>
namespace Plusnx::NoGui {
    class SdlVulkanBacking final : public SdlWindow, public Video::GraphicsSupportContext {
    public:
        SdlVulkanBacking();
        ~SdlVulkanBacking() override;

        void ActivateContext(const vk::Instance& context) override;
        void Update() override;
    };
}
