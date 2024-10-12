#include <SDL2/SDL.h>

#include <types.h>
#include <application.h>

Plusnx::i32 main() {
    SDL_Init(SDL_INIT_VIDEO);

    const auto window{SDL_CreateWindow("Plusnx", 0, 0, 800, 800, SDL_WINDOW_VULKAN)};
    auto app{std::make_unique<Plusnx::Application>()};

    SDL_DestroyWindow(window);

    SDL_Quit();
}
