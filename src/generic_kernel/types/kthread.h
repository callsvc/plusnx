#pragma once

#include <generic_kernel/kernel.h>
namespace Plusnx::GenericKernel::Types {
    class KThread : public KBaseType {
    public:
        explicit KThread(Kernel& kernel) : KBaseType(kernel, KType::KThread) {}
        void Initialize();

        u64 threadId{};
    };
}
