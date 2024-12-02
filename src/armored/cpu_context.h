#pragma once

#include <armored/arm_types.h>
namespace Plusnx::Armored {
    class CpuContext {
    public:
        CpuContext();
        u64 ccid;
        u64 vaddr{}; // Where in main memory (in pages) the current PC is pointing to

        ArmRegistersContext ctx; // All machine registers
        HostRegistersContext hrc;
    };
}