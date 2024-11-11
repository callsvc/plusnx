#include <print>

#include <SDL2/SDL_vulkan.h>
#include <nogui/sdl_vulkan_backing.h>
namespace Plusnx::NoGui {
    constexpr auto VulkanSdlFlags{SDL_WINDOW_VULKAN};
    SdlVulkanBacking::SdlVulkanBacking() : SdlWindow(VulkanSdlFlags | DefaultWindowFlags) {
        u32 count;
        SDL_Vulkan_GetInstanceExtensions(window, &count, nullptr);
        required.resize(count);

        SDL_Vulkan_GetInstanceExtensions(window, &count, required.data());
        for (const auto& supported : required) {
            std::print("SDL Vulkan: Extension supported by the layer {}\n", supported);
        }

        type = Video::BackendType::Vulkan;
        apiType = Video::ApiType::Sdl;

#if 0
        SDL_ShowWindow(window);
#endif
    }

    SdlVulkanBacking::~SdlVulkanBacking() {
        vkDestroySurfaceKHR(instance, surface, nullptr);
    }

    // https://wiki.libsdl.org/SDL2/SDL_Vulkan_CreateSurface
    void SdlVulkanBacking::ActivateContext(const vk::Instance& context) {
        if ((instance = context))
            if (SDL_Vulkan_CreateSurface(window, instance, &surface) != SDL_TRUE)
                throw Except("{}", SDL_GetError());
    }

    void SdlVulkanBacking::Update() {
        SDL_UpdateWindowSurface(window);
    }
}
