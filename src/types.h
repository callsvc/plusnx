#pragma once
#include <cstdint>
#include <cassert>
#include <string>

#include <boost/algorithm/hex.hpp>

#include <container.h>
#include <except.h>
namespace Plusnx {
    using u64 = std::uint64_t;

    using u32 = std::uint32_t;
    using i32 = std::int32_t;

    using u8 = std::uint8_t;

    template <u64 Size>
    std::array<u8, Size> HexTextToByteArray(const std::string_view& container) {
        std::array<u8, Size> result{};
        assert(container.size() / 2 <= result.size());

        try {
            for (u64 index{}, ptr{}; index < container.size(); ptr++) {
                result[ptr] = boost::algorithm::detail::hex_char_to_int(container[index]) << 4;
                result[ptr] |= boost::algorithm::detail::hex_char_to_int(container[index + 1]);
                index += 2;
            }
        } catch ([[maybe_unused]] boost::exception& except) {
            return {};
        }

        return result;
    }

    bool IsWaylandPresent();
    std::string GetSdlVersion();
    std::string GetOsErrorString();
}