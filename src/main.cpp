#include <print>
#include <SDL2/SDL.h>
#include <boost/program_options.hpp>

#include <core/application.h>
#include <nogui/sdl_vulkan_backing.h>
using namespace Plusnx;
namespace po = boost::program_options;

void CheckDriversVersion() {
    std::print("SDL version: {}\n", GetSdlVersion());
    std::print("SDL driver name {}\n", SDL_GetCurrentVideoDriver());

    if (IsWaylandPresent())
        if (std::string_view(SDL_GetCurrentVideoDriver()) == "wayland")
            std::print("Wayland is still experimental, consider using an X11 session\n");
}

std::string game;
po::options_description desc("Plusnx options");

i32 main(const i32 argc, const char** argv) {
    auto sdlVkWindow{std::make_shared<NoGui::SdlVulkanBacking>()};
    CheckDriversVersion();
    if (!argc && argv)
        if (*argv)
            return strlen(*argv);

    const auto app{std::make_unique<Core::Application>()};
    app->Initialize(std::move(sdlVkWindow));

    desc.add_options()
        ("extract", "extract the content of an NSP/XCI game into a GameFS directory")
        ("collect-metrics", "Collects host system details to assist in bug reporting")
        ("game-name", po::value(&game), "specifies the name of the game to be loaded from the previously specified paths");

    po::variables_map vm;
    store(parse_command_line(argc, argv, desc), vm);
    vm.notify();

    if (vm.contains("collect-metrics")) {
        app->SaveUserInformation();
    }

    if (!game.empty() && vm.contains("extract")) {
        app->PickByName(game);
        app->ExtractIntoGameFs();
    } else {
        app->LoadAGameByIndex();
    }

    bool quit{};
    SDL_Event event;

    app->ClearUiEvents();

    while (!quit) {
        if (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT)
                quit = true;
        }
        app->UpdateFrame();

        SDL_GetPerformanceCounter();
#if 1
        static u64 count{};
        // Process at least 1000 interactions before exiting
        if (count++ == 1'000)
            quit = true;
        std::this_thread::sleep_for(3ms);
#else
        SDL_WaitEvent(&event);
#endif
    }
    return {};
}
