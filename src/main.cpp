#include <print>
#include <SDL2/SDL.h>

#include <core/application.h>

#include <nogui/sdl_vulkan_backing.h>
#include <video/vk/vulkan_types.h>

using namespace Plusnx;

void CheckDriversVersion() {
    std::print("SDL version: {}\n", GetSdlVersion());
    std::print("SDL driver name {}\n", SDL_GetCurrentVideoDriver());

    if (IsWaylandPresent())
        assert(std::string_view(SDL_GetCurrentVideoDriver()) == "wayland");
}

i32 main() {
    SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO);

    CheckDriversVersion();

    const NoGui::SdlVulkanBacking sdlContext;
    const Video::Vk::VkSupport support(sdlContext);

    const auto app{std::make_unique<Core::Application>()};
    app->Initialize(support);

    SDL_Quit();
}
