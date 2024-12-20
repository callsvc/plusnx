#pragma once

#include <armored/arm_types.h>
namespace Plusnx::Armored {
    class CpuContext {
    public:
        CpuContext();
        u64 ccid;
        const u8* vaddr{nullptr}; // Where in main memory (in pages) the current PC is pointing to
        const u8* vend{nullptr};

        ArmRegistersContext ctx; // All machine registers
        Arm64Register pstate{}; // The PSTATE register is a collection of fields which are mostly used by the OS. The user programs make use of the first four bits, which are marked as N,Z,C and V respectively
        Arm64Register tpidrel0{};
        Arm64Register fpsr{};

        HostRegistersContext host;
    };
}