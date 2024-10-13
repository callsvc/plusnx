#include <print>
#include <SDL2/SDL.h>

#include <types.h>
#include <application.h>

#include <nogui/sdl_vulkan_backing.h>
#include <video/vk/vulkan_types.h>

namespace Plusnx {
    bool IsWaylandPresent();
}

Plusnx::i32 main() {
    SDL_Init(SDL_INIT_VIDEO);
    std::print("SDL2 driver name {}\n", SDL_GetCurrentVideoDriver());
    if (Plusnx::IsWaylandPresent())
        assert(std::string_view(SDL_GetCurrentVideoDriver()) == "wayland");

    const Plusnx::NoGui::SdlVulkanBacking sdlContext;
    const Plusnx::Video::Vk::VkSupport support(sdlContext);

    const auto app{std::make_unique<Plusnx::Application>()};
    app->Initialize(support);

    SDL_Quit();
}
