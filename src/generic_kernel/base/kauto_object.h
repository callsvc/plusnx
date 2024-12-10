#pragma once

#include <generic_kernel/base/auto_types.h>
namespace Plusnx::GenericKernel::Base {
    class KAutoObject : public KBaseType {
    public:
        KAutoObject(Kernel& generic, KAutoType type);

        void IncrementReference();
        void DecrementReference();

    private:
        std::mutex refLock;
        i32 refs{1};
    };
    static_assert(sizeof(KAutoObject) > 0x30);

    class KSynchronizationObject : public KAutoObject {
    public:
        KSynchronizationObject(Kernel& generic, const KAutoType type) : KAutoObject(generic, type) {}
    };
}