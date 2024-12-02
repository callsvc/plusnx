#pragma once

#include <memory>
#include <types.h>
namespace Plusnx::Armored {
    // https://developer.arm.com/documentation/dui0473/m/overview-of-the-arm-architecture/arm-registers
    union Arm64Register {
        u64 r;
        u32 l;
    };
    struct ArmRegistersContext {
        union {
            std::array<Arm64Register, 13> gprList;

            Arm64Register r0;
            Arm64Register r1;
            Arm64Register r2;
            Arm64Register r3;
            Arm64Register r4;
            Arm64Register r5;
            Arm64Register r6;
            Arm64Register r7;
            Arm64Register r8;
            Arm64Register r9;
            Arm64Register r10;
            Arm64Register r11;
            Arm64Register r12;
        };

        Arm64Register sp; // Can be used as a general-purpose register in ARM state only
        Arm64Register lr; // Can be used as a general-purpose register in ARM state only
        Arm64Register pc;
        Arm64Register apsr; // One Application Program Status Register (APSR)
    };

    struct HostRegistersContext {
        u64 eax;
        u64 ebx;
        u64 ecx;
        u64 edx;
    };
}