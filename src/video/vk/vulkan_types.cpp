#include <video/vk/vulkan_types.h>

#include <nogui/sdl_vulkan_backing.h>

namespace Plusnx::Video::Vk {
    VkSupport::VkSupport(const NoGui::SdlVulkanBacking& sdlvk) :
        enableExt(sdlvk.required) {
    }
}
