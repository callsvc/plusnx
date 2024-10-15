#include <print>
#include <SDL2/SDL.h>

#include <types.h>
#include <application.h>

#include <nogui/sdl_vulkan_backing.h>
#include <video/vk/vulkan_types.h>

using namespace Plusnx;

void ChecksDriverVersion() {
    std::print("SDL version: {}\n", GetSDLVersion());
    std::print("SDL driver name {}\n", SDL_GetCurrentVideoDriver());

    if (IsWaylandPresent())
        assert(std::string_view(SDL_GetCurrentVideoDriver()) == "wayland");
}

i32 main() {
    SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO);

    ChecksDriverVersion();

    const NoGui::SdlVulkanBacking sdlContext;
    const Video::Vk::VkSupport support(sdlContext);

    const auto app{std::make_unique<Application>()};
    app->Initialize(support);

    SDL_Quit();
}
