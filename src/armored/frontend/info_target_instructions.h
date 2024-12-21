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
        InstructionSyncBarrier = 0xD5033F5F
    };

    bool IsArm64Code(u32 code);
    bool IsArm64FixedInstruction(u32 code);
}
