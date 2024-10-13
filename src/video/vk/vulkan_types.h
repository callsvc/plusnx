#pragma once

#include <vulkan/vulkan.hpp>

namespace Plusnx::NoGui {
    class SdlVulkanBacking;
}

namespace Plusnx::Video::Vk {

    class VkSupport {
    public:
        explicit VkSupport(const NoGui::SdlVulkanBacking& sdlvk);
        std::vector<const char*> enableExt;
    };
}
