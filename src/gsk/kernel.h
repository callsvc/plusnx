#pragma once

#include <memory>

#include <atomic>

#include <cpu/core_container.h>
#include <cpu/unit_types.h>
#include <types.h>
namespace Plusnx::Gsk {
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
        struct {
            std::atomic<u64> pid;
        } seed;

        std::array<Cpu::CoreContainer, Cpu::totalCoresCount> cpuCores;
    };

    class KBaseType {
    protected:
        KBaseType(Kernel& gsk, const KType base) : kernel(gsk), type(base) {}
        Kernel& kernel;
        KType type;
    };
}