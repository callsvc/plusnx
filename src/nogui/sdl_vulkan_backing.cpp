#include <print>

#include <SDL2/SDL_vulkan.h>
#include <nogui/sdl_vulkan_backing.h>
#include <types.h>

namespace Plusnx::NoGui {
    SdlVulkanBacking::SdlVulkanBacking() :
        window(SDL_CreateWindow("Plusnx", 0, 0, 800, 800, SDL_WINDOW_VULKAN)) {
        u32 count;
        SDL_Vulkan_GetInstanceExtensions(window, &count, nullptr);
        required.resize(count);

        SDL_Vulkan_GetInstanceExtensions(window, &count, required.data());

        for (const auto& supported : required) {
            std::print("SDL Vulkan: Extension supported by the layer {}\n", supported);
        }
    }

    SdlVulkanBacking::~SdlVulkanBacking() {
        SDL_DestroyWindow(window);
    }
}
