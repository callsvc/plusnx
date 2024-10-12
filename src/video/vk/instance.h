#pragma once

#include <video/vk/vulkan_types.h>
namespace Plusnx::Video::Vk {
    class Instance {
    public:
        Instance();
        ~Instance();

        std::optional<vk::Instance> context;
    };
}
