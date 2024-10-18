#pragma once

#include <atomic>
#include <types.h>
namespace Plusnx::Kmo {
    enum class KType {
        KProcess,
        KThread
    };
    class Kernel {
    public:
        Kernel() = default;
        u64 CreateProcessId();
    private:
        std::atomic<u64> pitch;
    };

    class KBaseType {
    protected:
        KBaseType(Kernel& kmo, const KType base) : kernel(kmo), type(base) {}
        Kernel& kernel;
        KType type;
    };
}