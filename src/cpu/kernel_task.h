#pragma once

namespace Plusnx::Nxk {
    class Kernel;
}

#include <types.h>
namespace Plusnx::Cpu {
    class CoreContainer;

    class KernelTask {
    public:
        KernelTask(Nxk::Kernel& _kernel) : kernel(_kernel) {}

        bool CheckForActivation(const CoreContainer& multicore);
        void DeactivateCore(CoreContainer& multicore) const;
        void DeactivateCore() const;
        bool Run() const;

        Nxk::Kernel& kernel;
        u64 activated{};
    };
}
