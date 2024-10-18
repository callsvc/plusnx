#include <kmo/kernel.h>
namespace Plusnx::Kmo {
    Kernel::Kernel() {
        for (u32 core{}; core < Cpu::totalCoresCount; core++)
            cpuCores[core].Initialize();
    }

    Kernel::~Kernel() {
        for (u32 core{}; core < Cpu::totalCoresCount; core++)
            cpuCores[core].Destroy();
    }

    u64 Kernel::CreateProcessId() {
        return pitch++;
    }

}