#pragma once

#include <memory>

#include <list>
#include <atomic>

#include <cpu/core_container.h>
#include <generic_kernel/user_space.h>

namespace Plusnx::GenericKernel {
    namespace Types {
        class KProcess;
        class KSharedMemory;
    }
    enum class KType {
        KProcess,
        KThread,
        KSharedMemory
    };

    class Kernel {
    public:
        Kernel();
        ~Kernel();

        std::shared_ptr<Types::KProcess> CreateNewProcess();
        u64 CreateProcessId();

        std::unique_ptr<UserSpace> userspace;
    private:
        struct {
            std::atomic<u64> pid, tid;
        } seed;

        std::array<Cpu::CoreContainer, Cpu::TotalCoresCount> cpuCores;
        std::list<std::shared_ptr<Types::KProcess>> listProc;
    };

    class KBaseType {
    protected:
        KBaseType(Kernel& generic, const KType base) : kernel(generic), type(base) {}
        Kernel& kernel;
        KType type;
    };
}
