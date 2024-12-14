#pragma once

#include <types.h>
namespace Plusnx::Nxk {
    class Kernel;
}

namespace Plusnx::Nxk::Base {
    enum class KAutoType {
        KProcess,
        KThread,
        KSharedMemory
    };

    class KBaseType {
    public:
        KBaseType(Kernel& generic, const KAutoType base) : kernel(generic), type(base) {}
        virtual ~KBaseType() = default;
        Kernel& kernel;
        KAutoType type;
    };

}