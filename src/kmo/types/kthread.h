#pragma once

#include <kmo/kernel.h>
namespace Plusnx::Kmo::Types {
    class KThread : KBaseType {
    public:
        KThread(Kernel& kernel) : KBaseType(kernel, KType::KThread) {}
    };
}
