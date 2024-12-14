#pragma once
#include <cstdint>
#include <cassert>
#include <string>

#include <boost/algorithm/hex.hpp>

#include <container.h>
#include <runtime_exception.h>
namespace Plusnx {
    using u64 = std::uint64_t;
    using i64 = std::int64_t;

    using u32 = std::uint32_t;
    using i32 = std::int32_t;

    using u16 = std::uint16_t;
    using i16 = std::int16_t;
    using u8 = std::uint8_t;
    using i8 = std::int8_t;

    template<typename T> requires(std::is_integral_v<T>)
    class Range {
    public:
        Range() = default;
        Range(T _min, T _max) : min(_min), max(_max) {
            assert(min < max);
        }

        std::vector<T> CreateValues(u64 count) {
            std::vector<T> result;
            if (!(max - min))
                return result;
            if (count > max - min)
                result.reserve(count);
            else
                result.resize(max - min);
            for (u64 pos{min}; pos < max; ++pos)
                result.emplace(pos);
            return result;
        }

        operator std::string() const {
            return std::format("{} to {}", min, max);
        }
    private:
        T min{};
        T max{};
    };

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
        } catch ([[maybe_unused]] const boost::exception& except) {
            return {};
        }

        return result;
    }
    template <typename T, u64 Size = sizeof(T)>
    auto TypeValueFromArray(const std::array<u8, Size>& container) {
        assert(container.size() >= sizeof(T));
        T value{};
        std::memcpy(&value, container.data(), sizeof(T));
        return value;
    }

    void ActivateTrap();
    bool IsWaylandPresent();
    std::string GetSdlVersion();
    std::string GetOsErrorString();
}