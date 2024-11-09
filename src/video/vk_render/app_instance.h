#pragma once

#include <video/graphics_types.h>
namespace Plusnx::Video::VkRender {
    class AppInstance {
    public:
        explicit AppInstance(const std::vector<std::string_view>& required);
        ~AppInstance();

        vk::Instance instance;
    private:
        std::vector<const char*> extensions;
        std::vector<const char*> layers;

        void EnumerateSupportLayers();
        void EnumerateRequiredExtensions(const std::vector<std::string_view>& required);
    };
}
