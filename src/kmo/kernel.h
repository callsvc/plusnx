#pragma once

#include <memory>

#include <atomic>

#include <cpu/core_container.h>
#include <cpu/unit_types.h>
#include <types.h>
namespace Plusnx::Kmo {
    enum class KType {
        KProcess,
        KThread
    };
    class Kernel {
    public:
        Kernel();
        ~Kernel();
        u64 CreateProcessId();
    private:
        std::atomic<u64> pitch;

        std::array<Cpu::CoreContainer, Cpu::totalCoresCount> cpuCores;
    };

    class KBaseType {
    protected:
        KBaseType(Kernel& kmo, const KType base) : kernel(kmo), type(base) {}
        Kernel& kernel;
        KType type;
    };
}