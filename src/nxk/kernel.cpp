#include <sys/resource.h>

#include <nxk/types/kprocess.h>
#include <nxk/kernel.h>
namespace Plusnx::Nxk {
    Kernel::Kernel() {
        for (u32 core{}; core < Cpu::TotalCoresCount; core++) {
            cpuCores[core].emplace(core, *this);
            cpuCores[core]->Initialize();
        }

        // We need the maximum possible CPU time
        const auto type{sched_getscheduler(0)};
        if (sched_get_priority_min(type) == sched_get_priority_min(type)) {
            assert(getpriority(PRIO_PROCESS, 0) == 0);
            nice(PRIO_MIN);
            if (errno == EPERM)
                nice(0);
        }

        scheduler.emplace(*this);

        nxmemory = std::make_unique<MemoryNx>(*this);
        user = std::make_unique<UserSpace>(nxmemory);
        slabHeap = std::make_unique<Memory::KSlabHeap>(*this, UserSlabBase, UserSlabHeapItemSize, UserSlabHeapSize);
    }

    Kernel::~Kernel() {
        for (u32 core{}; core < Cpu::TotalCoresCount; core++)
            cpuCores[core]->Destroy();
    }

    std::shared_ptr<Types::KProcess> Kernel::CreateNewProcess() {
        listProc.push_back(std::make_shared<Types::KProcess>(*this));
        return listProc.back();
    }

    u64 Kernel::CreateProcessId() {
        return processId++;
    }

    u64 Kernel::CreateThreadId() {
        std::array<char, 16> thread{};
        pthread_getname_np(pthread_self(), thread.data(), thread.size());

        std::print("A new HOS thread is being created by the thread named {} on core {}\n", std::string_view{thread.data()}, sched_getcpu());

        return threadId++;
    }

    u64 Kernel::SetupApplicationProcess(const u64 pid) {
        u64 core{};

        for (const auto& process : listProc) {
            const auto numCore{process->npdm.titleNpdm.defaultCoreId};
            if (process->pid != pid)
                continue;

            assert(process->threads.empty());

            core = cpuCores[numCore]->cpusched;
            corePid.emplace(core, pid);
            cpuCores[numCore]->Enabled(true);
        }
        return core;
    }

    std::shared_ptr<Types::KProcess> Kernel::GetCurrentProcess() {
        return listProc.back();
    }
}
