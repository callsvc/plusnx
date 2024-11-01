#pragma once

#include <video/vk/api_types.h>
namespace Plusnx::Video::Vk {
    class AppInstance {
    public:
        explicit AppInstance(const std::vector<std::string_view>& required);
        ~AppInstance();

        vk::Instance vkInstance;
    private:
        std::vector<const char*> extensions;
        std::vector<const char*> layers;

        void EnumerateSupportLayers();
        void EnumerateRequiredExtensions(const std::vector<std::string_view>& required);
    };
}
