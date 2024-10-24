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

    template <typename T>
    auto ConstMagic(const std::string_view& string) {
        T value;
        std::memcpy(&value, string.data(), sizeof(value));
        return value;
    }

    template <typename T>
    auto StringViewBuilder(const std::span<T>& string) {
        return std::string_view(reinterpret_cast<const char*>(string.data()), string.size());
    }
}
