#pragma once

#include <boost/container/small_vector.hpp>
#include <capstone/capstone.h>

#include <types.h>
namespace Plusnx::Armored {
    enum class DisasFlavourType {
        Arm64,
        Arm32,
        X86_64
    };
    class ArmX86Disas {
    public:
        ArmX86Disas(DisasFlavourType flavour, bool half = false);
        ~ArmX86Disas();
        std::vector<std::string> to_string(const std::span<u8>& code);

        std::string to_string(const u32 code) {
            boost::container::small_vector<u8, 4> backing(4);
            std::memcpy(backing.data(), &code, sizeof(code));
            if (const auto result{to_string(std::span(backing))}; !result.empty())
                return result.front();
            return {};
        }

        operator bool() const;

        u64 relativePc{};
        bool thumb{false};

        csh handle{};
        DisasFlavourType type;
    };
}
