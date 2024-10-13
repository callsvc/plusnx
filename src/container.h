#pragma once
#include <vector>

namespace Plusnx {
    template <typename T>
    auto ContainsValue(const std::vector<T>& container, const T& value) -> bool {
        auto result{std::ranges::find(container, value)};
        return result != std::end(container);
    }
}
