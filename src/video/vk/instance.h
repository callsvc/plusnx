#pragma once

#include <video/vk/vulkan_types.h>
namespace Plusnx::Video::Vk {
    class Instance {
    public:
        explicit Instance(const std::vector<std::string_view>& required);
        ~Instance();

        std::optional<vk::Instance> instance;

    private:
        std::vector<const char*> extensions;
        std::vector<const char*> layers;

        void EnumerateSupportLayers();
        void EnumerateRequiredExtensions(const std::vector<std::string_view>& required);
    };
}
