#include <bitset>
#include <ranges>
#include <vector>

#include <armored/frontend/arm64_faster_intrusive_decoder.h>
#include <details/bits.h>

namespace Plusnx::Armored::Frontend {

    // ReSharper disable once CppDFAConstantParameter
    constexpr Arm64Format::Arm64Format(const Arm64FormatType _type, const std::string_view& format) : type(_type) {
        auto it{format.begin()};
        u32 constbits{};

        u32 counter{};
        u32 value{};
        while (it != format.end() && counter != 32) {
            assert(counter < 32);

            switch (*it) {
                case 'C':
                    ++it;

                    constbits = boost::algorithm::detail::hex_char_to_int(*it++) << 4;
                    constbits |= boost::algorithm::detail::hex_char_to_int(*it++);
                    assert(*it++ == '-');

                    value = std::strtoll(it, nullptr, 0x10);
                    _desc.emplace_back(DescType::Constant, value, counter, constbits);
                    counter = _desc.back()._bits.max;
                    break;
                default:
                    ++it;
            }
        }
    }

    i32 Arm64Format::GetConstantBits() const {
        i32 result{};
        for (const auto& _const : _desc) {
            if (_const.type != DescType::Constant)
                continue;
            result += std::countr_one(_const.value);

        }
        return result;
    }

    bool MatcherFormat::Matches(const u32 _inst, const Arm64Format& fmt) {
        for (const auto& [index, _desc] : std::views::enumerate(fmt._desc)) {
            switch (_desc.type) {
                case DescType::Constant:
                    if (Details::CompareBitsRange<u32, u8>(_inst, _desc.value, _desc._bits))
                        return true;
                    break;
                default: {}
            }
        }
        return {};
    }

    std::optional<Arm64Format> FasterArm64IntrusiveDecoder::Decode(const u32 code) {
        static const std::vector<Arm64Format> table{
            {Arm64FormatType::Nop, "C20-D503201F"},
        };

        [[unlikely]] if (_constNearest.empty()) {
            _constNearest.reserve(table.size());

            for (const auto& _inst64fmt : table) {
                _constNearest.insert(std::ranges::upper_bound(_constNearest, _inst64fmt, [](const auto& fmt1, const auto& fmt2) {
                    return fmt1.GetConstantBits() < fmt2.GetConstantBits();
                }), _inst64fmt);
            }
        }
        if (std::countr_one(code) >= std::prev(_constNearest.end())->GetConstantBits()) {
            for (const auto& _a64near : _constNearest) {
                if (MatcherFormat::Matches(code, _a64near))
                    return _a64near;
            }
        }

        return {};
    }
}
