#pragma once

#include <boost/container/small_vector.hpp>

#include <types.h>
namespace Plusnx::Armored::Frontend {
    enum class DescType {
        None,
        Constant
    };
    struct Descriptor {
        Descriptor(const DescType _desc, const u32 val, const u8 begin = 0, const u8 end = 0) : type(_desc), _bits(begin, end), value(val) {}

        DescType type;
        Range<u8> _bits;
        u32 value;
    };

    enum class Arm64FormatType {
        None,
        Nop
    };
    struct Arm64Format {
        constexpr Arm64Format(Arm64FormatType _type, const std::string_view& format);

        i32 GetConstantBits() const;
        Arm64FormatType type;
        boost::container::small_vector<Descriptor, 8> _desc;
    };

    struct MatcherFormat {
        static bool Matches(u32 _inst, const Arm64Format& fmt);
    };

    class FasterArm64IntrusiveDecoder {
    public:
        FasterArm64IntrusiveDecoder() = default;

        std::optional<Arm64Format> Decode(u32 code);

        std::vector<Arm64Format> _constNearest{};
    };
}
