#pragma once
#include <cstring>
#include <span>
#include <vector>

namespace Plusnx {
    template <typename T>
    auto ContainsValue(const auto& container, const T& value) -> bool {
        auto result{std::ranges::find(container, value)};
        return result != container.end();
    }

    template <typename T> requires std::is_integral_v<T>
    constexpr auto ConstMagic(const std::string_view& string) {
        T value{};
        T count{};
        for (const auto& byte : string) {
            value |= static_cast<std::uint8_t>(byte) << count++ * 8;
        }

        return value;
    }

    template <typename T>
    auto StringViewBuilder(const std::span<T>& string) {
        return std::string_view(reinterpret_cast<const char*>(string.data()), string.size());
    }

    template <typename T>
    bool IsEmpty(const T& container) {
        return std::ranges::empty(container);
    }
    template <typename T> requires std::is_trivial_v<T>
    bool IsValueEmpty(const T& value) {
        T empty{};
        return std::memcmp(&value, &empty, sizeof(T)) == 0;
    }

    template <typename T, typename B>
    bool IsEqual(const T& left, const B& right) {
        return std::ranges::equal(left, right);
    }
}
