#pragma once

#include <memory>
#include <types.h>
namespace Plusnx::Armored {
    // https://developer.arm.com/documentation/dui0473/m/overview-of-the-arm-architecture/arm-registers
    // https://valsamaras.medium.com/arm-64-assembly-series-basic-definitions-and-registers-ec8cc1334e40
    union Arm64Register {
        u64 X; u32 W;
    };
    struct ArmRegistersContext {
        union {
            std::array<Arm64Register, 28> gprs;
            Arm64Register r0, r1, r2, r3, r4, r5, r6, r7, r8;
        };

        Arm64Register fp;
        Arm64Register lr;
        Arm64Register sp; // Can be used as a general-purpose register in ARM state only
        Arm64Register pc;
    };

    struct HostRegistersContext {
        u64 eax;
        u64 ebx;
        u64 ecx;
        u64 edx;
    };
}