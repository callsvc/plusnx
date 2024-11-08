#include <SDL2/SDL.h>

#include <types.h>
#include <nogui/sdl_window.h>
namespace Plusnx::NoGui {
    constexpr auto SdlTitle{"Plusnx"};

    SdlWindow::SdlWindow(const i32 flags) {
        assert(SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO) == 0);
        if (!(flags & SDL_WINDOW_VULKAN) && !(flags & SDL_WINDOW_OPENGL))
            throw Except("No valid backend was specified");

        window = SDL_CreateWindow(SdlTitle, 0, 0, 800, 800, flags);
    }

    SdlWindow::~SdlWindow() {
        SDL_DestroyWindow(window);
        SDL_Quit();
    }
}
