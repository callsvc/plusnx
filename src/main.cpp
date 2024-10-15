#include <print>
#include <SDL2/SDL.h>

#include <types.h>
#include <application.h>

#include <nogui/sdl_vulkan_backing.h>
#include <video/vk/vulkan_types.h>

using namespace Plusnx;

i32 main() {
    SDL_Init(SDL_INIT_VIDEO);
    std::print("SDL2 driver name {}\n", SDL_GetCurrentVideoDriver());
    if (IsWaylandPresent())
        assert(std::string_view(SDL_GetCurrentVideoDriver()) == "wayland");

    const NoGui::SdlVulkanBacking sdlContext;
    const Video::Vk::VkSupport support(sdlContext);

    const auto app{std::make_unique<Application>()};
    app->Initialize(support);

    SDL_Quit();
}
