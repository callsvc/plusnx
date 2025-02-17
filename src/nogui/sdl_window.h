#pragma once

#include <SDL2/SDL.h>
#include <types.h>
namespace Plusnx::NoGui {
    constexpr auto DefaultWindowFlags{SDL_WINDOW_HIDDEN | SDL_WINDOW_ALLOW_HIGHDPI};
    class SdlWindow {
    public:
        SdlWindow(i32 flags);

        virtual ~SdlWindow();

        SDL_Window* window{nullptr};
    };
}