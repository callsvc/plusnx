#pragma once

#include <generic_kernel/base/kauto_object.h>
#include <generic_kernel/kernel.h>
namespace Plusnx::GenericKernel::Types {
    class KThread : public Base::KSynchronizationObject {
    public:
        explicit KThread(Kernel& kernel) : KSynchronizationObject(kernel, Base::KAutoType::KThread) {}
        void Initialize();

        u64 threadId{};
    };
}
