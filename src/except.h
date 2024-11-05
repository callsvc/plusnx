#pragma once
#include <stdexcept>
#include <format>
#include <chrono>
#define PLUSNX_GUI_SDL2 1

#if PLUSNX_GUI_SDL2
#include <SDL2/SDL.h>
#endif

namespace Plusnx {
    using namespace std::chrono_literals;
    class Except final : public std::runtime_error {
    public:
        template <typename... Args>
        Except(const std::format_string<Args...>& format, Args&&... args) :
            std::runtime_error(std::format(format, std::forward<Args...>(args)...)) {
#if PLUSNX_GUI_SDL2
            SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "An exception was thrown", runtime_error::what(), nullptr);
#endif
        }
    };
}