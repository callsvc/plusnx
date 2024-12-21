#include <armored/arm_x86_disas.h>

namespace Plusnx::Armored {
    ArmX86Disas::ArmX86Disas(const DisasFlavourType flavour) : type(flavour) {}

    ArmX86Disas::~ArmX86Disas() {
    }

    // ReSharper disable once CppMemberFunctionMayBeStatic
    std::string ArmX86Disas::to_string([[maybe_unused]] const std::span<u8>& code) {
        return {};
    }
}
