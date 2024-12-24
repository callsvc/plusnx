
#include <armored/arm_x86_disas.h>
#include <armored/frontend/info_target_instructions.h>
namespace Plusnx::Armored::Frontend {
    bool IsArm64Code(const u32 code) {
        static ArmX86Disas cpuDisas(DisasFlavourType::Arm64);

        if (const auto instruction{cpuDisas.to_string(code)}; !instruction.empty()) {
            std::println("{:X} -> {}", code, instruction);
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
            case OpcodeArm64::Return:
            case OpcodeArm64::DebugRestoreProcess:
                return true;
            default:
                return {};
        }
    }

    bool IsArm64Ret(const u32 code) {
        if ((code >> 16 & 0xD65F) == 0xD65F)
            return true;
        return {};
    }
    bool IrArm64BranchFlavour(const u32 code) {
        // Unconditional jump to a register, RET (Return from a subroutine)
        if (IsArm64Ret(code))
            return true;

        if ((code >> 24 & 0x54) == 0x54)
            return true;
        if ((code >> 25 & 0x6B) == 0x6B)
            if (code != std::to_underlying(OpcodeArm64::DebugRestoreProcess))
                return true;
        if ((code >> 26 & 0x5) == 0x5)
            return true;
        // Branch with Link
        if ((code >> 26 & 0x25) == 0x25)
            return true;

        return {};
    }
}
