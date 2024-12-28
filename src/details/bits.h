#pragma once

#include <types.h>
namespace Plusnx::Details {
    template <typename T, typename R>
    bool CompareBitsRange(T value, T mask, const Range<R>& range) {
        std::bitset<sizeof(value) * 8> set{value};
        for (u32 idx{range.min}; idx < range.max; ++idx) {
            if (set.test(idx) != mask << idx)
                return {};
        }
        return true;
    }
}