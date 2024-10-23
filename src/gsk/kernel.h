#pragma once

#include <memory>

#include <list>
#include <atomic>

#include <cpu/core_container.h>
namespace Plusnx::Gsk {
    namespace Types {
        class KProcess;
    }
    enum class KType {
        KProcess,
        KThread
    };

    class Kernel {
    public:
        Kernel();
        ~Kernel();

        std::shared_ptr<Types::KProcess> CreateNewProcess();
        u64 CreateProcessId();
    private:
        struct {
            std::atomic<u64> pid, tid;
        } seed;

        std::array<Cpu::CoreContainer, Cpu::TotalCoresCount> cpuCores;
        std::list<std::shared_ptr<Types::KProcess>> listProc;
    };

    class KBaseType {
    protected:
        KBaseType(Kernel& gsk, const KType base) : kernel(gsk), type(base) {}
        Kernel& kernel;
        KType type;
    };
}