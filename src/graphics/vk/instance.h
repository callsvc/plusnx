#pragma once

#include <graphics/vk/vulkan_types.h>
namespace Plusnx::Graphics::Vk {
    class Instance {
        public:
        Instance();

        std::optional<vk::Instance> context;
    };
}
