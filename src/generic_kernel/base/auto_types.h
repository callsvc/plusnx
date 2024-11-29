#pragma once

#include <types.h>
namespace Plusnx::GenericKernel {
    class Kernel;
}

namespace Plusnx::GenericKernel::Base {
    enum class KAutoType {
        KProcess,
        KThread,
        KSharedMemory
    };

    class KBaseType {
    protected:
        KBaseType(Kernel& generic, const KAutoType base) : kernel(generic), type(base) {}
        Kernel& kernel;
        KAutoType type;
    };

    static_assert(sizeof(KBaseType) == 0x10);

    class KSynchronizationObject : public KBaseType {
    public:
        KSynchronizationObject(Kernel& generic, const KAutoType type) : KBaseType(generic, type) {}
    };
}