
#include <armored/arm_x86_disas.h>
#include <armored/frontend/info_target_instructions.h>
namespace Plusnx::Armored::Frontend {
    bool IsArm64Code(const u32 code) {
        static ArmX86Disas cpuDisas(DisasFlavourType::Arm64);

        if (const auto instruction{cpuDisas.to_string(code)}; !instruction.empty()) {
            std::println(" {:X} : {} ", code, instruction);
            return true;
        }
        if (cpuDisas)
            return {};

        auto result{true};
        if (IsArm64FixedInstruction(code))
            result = true;
        return result;
    }

    bool IsArm64FixedInstruction(const u32 code) {
        switch (static_cast<OpcodeArm64>(code)) {
            case OpcodeArm64::NoOperation:
            case OpcodeArm64::WaitForInt:
            case OpcodeArm64::SendEvent:
            case OpcodeArm64::Break:
            case OpcodeArm64::DataMemoryBarrier:
            case OpcodeArm64::DataSyncBarrier:
            case OpcodeArm64::InstructionSyncBarrier:
                return true;
            default:
                return {};
        }
    }
}
