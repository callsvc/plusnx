#pragma once

#include <types.h>
namespace Plusnx::Security {
    enum class KeyGeneration : u8 {
        Key0900 = 0xA,
        Key0910, // 09.1.0
        Key1210, // 12.1.0
        Key1300, // 13.0.0
        Key1400, // 14.0.0
        Key1500, // 15.0.0
        Key1600, // 16.0.0
        Key1700, // 17.0.0
        Key1800, // 18.0.0
        Key1900, // 19.0.0

        KeyInvalid = 0xFF
    };

    using RightsId = std::array<u8, 0x10>;
    using InitVector = std::array<u8, 16>;
    using K128 = std::array<u8, 16>;
    using K256 = std::array<u8, 32>;

    std::array<u8, 16> GetSwitchCounter(u64 counter);
    std::string GetMbedError(i32 error);
}