#include <print>
#include <SDL2/SDL.h>
#include <boost/program_options.hpp>

#include <core/application.h>
#include <nogui/sdl_vulkan_backing.h>
#include <video/vk/api_types.h>

using namespace Plusnx;
namespace po = boost::program_options;

void CheckDriversVersion() {
    std::print("SDL version: {}\n", GetSdlVersion());
    std::print("SDL driver name {}\n", SDL_GetCurrentVideoDriver());

    if (IsWaylandPresent())
        assert(std::string_view(SDL_GetCurrentVideoDriver()) == "wayland");
}

bool extract;
std::string game;
po::options_description desc("Plusnx options");

i32 main(const i32 argc, const char** argv) {
    SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO);

    CheckDriversVersion();

    const NoGui::SdlVulkanBacking sdlContext;
    const Video::Vk::VkSupport support(sdlContext);

    const auto app{std::make_unique<Core::Application>()};
    app->Initialize(support);

    desc.add_options()
        ("extract", po::bool_switch(&extract), "extract the content of an NSP/XCI game into a GameFS directory")
        ("game-name", po::value(&game), "specifies the name of the game to be loaded from the previously specified paths");

    po::variables_map vm;
    store(parse_command_line(argc, argv, desc), vm);
    vm.notify();

    if (!game.empty() && extract) {
        app->PickByName(game);
        app->ExtractIntoGameFs();
    } else {
        app->LoadAGameByIndex();
    }

    SDL_Quit();
}
