#pragma once

namespace Plusnx::GenericKernel {
    class Kernel;
}

#include <types.h>
namespace Plusnx::Cpu {
    class CoreContainer;

    class KernelTask {
    public:
        KernelTask(GenericKernel::Kernel& _kernel) : kernel(_kernel) {}

        bool CheckForActivation(const CoreContainer& multicore);
        void DeactivateCore(CoreContainer& multicore) const;
        void DeactivateCore() const;
        bool Run() const;

        GenericKernel::Kernel& kernel;
        u64 activated{};
    };
}
