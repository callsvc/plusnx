#pragma once

#include <types.h>
namespace Plusnx::Armored::Frontend {
    enum class OpcodeArm64 : u32 {
        NoOperation = 0xD503201F,
        WaitForInt = 0xD503205F,
        SendEvent = 0xD503209F,
        Break = 0xD4200000,
        DataMemoryBarrier = 0xD5033FDF,
        DataSyncBarrier = 0xD5033F9F,
        InstructionSyncBarrier = 0xD5033F5F,
        Return = 0xD65F03C0, // Fixed instruction using the default register X30
        DebugRestoreProcess = 0xD6BF03E0,
    };

    bool IsArm64Code(u32 code);
    bool IsArm64FixedInstruction(u32 code);
    bool IrArm64BranchFlavour(u32 code);
    bool IsArm64Ret(u32 code);
}
