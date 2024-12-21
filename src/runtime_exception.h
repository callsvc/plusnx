#pragma once
#include <stdexcept>
#include <format>
#include <chrono>
#include <print>
#define PLUSNX_GUI_SDL2 1

#if PLUSNX_GUI_SDL2
#include <SDL2/SDL.h>
#endif

namespace Plusnx {
    using namespace std::chrono_literals;
    class exception final : public std::runtime_error {
    public:
        template <typename... Args>
        exception(const std::format_string<Args...>& format, Args&&... args) :
            std::runtime_error(std::format(format, std::forward<Args>(args)...)) {
            PrintPrettyMessage();
        }
        exception(const std::string& error) : std::runtime_error(error) {
            PrintPrettyMessage();
        }

        void PrintPrettyMessage() const;
        static std::vector<std::string> GetStackTrace();
    };
}