#pragma once

#include <types.h>
namespace Plusnx::Nxk {
    class Kernel;
}
namespace Plusnx::Cpu {
    class CoreBlob;

    class KernelTask {
    public:
        KernelTask(Nxk::Kernel& _kernel, const u64 cpu) : kernel(_kernel), cpusched(cpu) {}

        bool CheckForActivation(const CoreBlob& multicore) const;
        void DeactivateCore(CoreBlob& multicore) const;
        void DeactivateCore() const;
        bool PreemptAndRun() const;

        Nxk::Kernel& kernel;
        u64 cpusched{};
    };
}
