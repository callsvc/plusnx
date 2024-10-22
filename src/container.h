#pragma once
#include <cstring>
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
}
