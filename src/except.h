#pragma once
#include <stdexcept>
#include <format>

namespace Plusnx {
    class Except final : public std::runtime_error {
    public:
        template <typename... Args>
        Except(const std::format_string<Args...>& format, Args&&... args) :
            std::runtime_error(std::format(format, std::forward<Args...>(args)...)) {}
    };
}