#pragma once
#include <vector>

namespace Plusnx {
    template <typename T>
    auto ContainsValue(const auto& container, const T& value) -> bool {
        auto result{std::ranges::find(container, value)};
        return result != container.end();
    }
}
