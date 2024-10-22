#include <gsk/kernel.h>
namespace Plusnx::Gsk {
    Kernel::Kernel() {
        for (u32 core{}; core < Cpu::TotalCoresCount; core++)
            cpuCores[core].Initialize();
    }

    Kernel::~Kernel() {
        for (u32 core{}; core < Cpu::TotalCoresCount; core++)
            cpuCores[core].Destroy();
    }

    u64 Kernel::CreateProcessId() {
        return seed.pid++;
    }

}