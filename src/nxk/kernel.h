#pragma once

#include <memory>

#include <list>
#include <atomic>

#include <cpu/core_blob.h>
#include <nxk/user_space.h>
#include <nxk/scheduler_preemptive.h>
#include <nxk/memory/k_slab_heap.h>
namespace Plusnx::Nxk {
    namespace Types {
        class KProcess;
        class KSharedMemory;
    }

    class Kernel {
    public:
        Kernel();
        ~Kernel();

        std::shared_ptr<Types::KProcess> CreateNewProcess();
        std::shared_ptr<Types::KProcess> GetCurrentProcess();

        u64 CreateProcessId();
        u64 CreateThreadId();
        u64 SetupApplicationProcess(u64 pid);

        std::unique_ptr<MemoryNx> nxmemory;
        std::unique_ptr<UserSpace> user;

        std::unique_ptr<Memory::KSlabHeap> slabHeap;

        std::map<u64, u64> corePid;
        std::optional<SchedulerPreemptive> scheduler;
    private:
        std::atomic<u64> processId, threadId;

        std::array<std::optional<Cpu::CoreBlob>, Cpu::TotalCoresCount> cpuCores;
        std::list<std::shared_ptr<Types::KProcess>> listProc;
    };

}
