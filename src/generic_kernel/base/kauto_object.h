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
        i32 refs{};
    };
    static_assert(sizeof(KAutoObject) > 0x30);
}