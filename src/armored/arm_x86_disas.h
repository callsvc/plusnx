#pragma once

#include <boost/container/small_vector.hpp>
#include <types.h>

namespace Plusnx::Armored {
    enum class DisasFlavourType {
        Arm64,
        Arm32,
        X86_64
    };
    class ArmX86Disas {
    public:
        ArmX86Disas(DisasFlavourType flavour);
        ~ArmX86Disas();
        std::string to_string(const std::span<u8>& instruction);
        std::string to_string(const u32 code) {
            boost::container::small_vector<u8, 4> backing;
            std::memcpy(backing.data(), &code, sizeof(code));
            return to_string(std::span(backing));
        }

        operator bool() const {
            return context != nullptr;
        }

        u64 pc64{};
        bool thumb{false};
        void* context{nullptr};
        DisasFlavourType type;
    };
}
