#pragma once

#include <generic_kernel/kernel.h>
namespace Plusnx::GenericKernel::Types {
    class KSharedMemory : public KBaseType {
    public:
        KSharedMemory(Kernel& kernel) : KBaseType(kernel, KType::KSharedMemory) {}
    };
}
