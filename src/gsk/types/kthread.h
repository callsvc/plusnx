#pragma once

#include <gsk/kernel.h>
namespace Plusnx::Gsk::Types {
    class KThread : KBaseType {
    public:
        explicit KThread(Kernel& kernel) : KBaseType(kernel, KType::KThread) {}
    };
}
