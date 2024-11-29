#include <sys/resource.h>

#include <generic_kernel/types/kprocess.h>
#include <generic_kernel/kernel.h>
namespace Plusnx::GenericKernel {
    Kernel::Kernel() {
        for (u32 core{}; core < Cpu::TotalCoresCount; core++)
            cpuCores[core].Initialize();

        // We need the maximum possible CPU time
        const auto type{sched_getscheduler(0)};
        if (sched_get_priority_min(type) == sched_get_priority_min(type)) {
            assert(getpriority(PRIO_PROCESS, 0) == 0);
            nice(PRIO_MIN);
            if (errno == EPERM)
                nice(0);
        }

        memory = std::make_unique<UserSpace>();
    }

    Kernel::~Kernel() {
        for (u32 core{}; core < Cpu::TotalCoresCount; core++)
            cpuCores[core].Destroy();
    }

    std::shared_ptr<Types::KProcess> Kernel::CreateNewProcess() {
        listProc.push_back(std::make_shared<Types::KProcess>(*this));
        return listProc.back();
    }

    u64 Kernel::CreateProcessId() {
        return seed.pid++;
    }

    std::shared_ptr<Types::KProcess> Kernel::GetCurrentProcess() {
        return listProc.back();
    }
}
