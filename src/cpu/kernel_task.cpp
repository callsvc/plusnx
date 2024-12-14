#include <nxk/kernel.h>
#include <nxk/types/kprocess.h>

#include <cpu/core_container.h>
#include <cpu/kernel_task.h>
namespace Plusnx::Cpu {
    bool KernelTask::CheckForActivation(const CoreContainer& multicore) {
        if (multicore.state != CoreState::Running) {
            multicore.state.wait(CoreState::Waiting);
        }
        if (!kernel.corePid.contains(multicore.coreId))
            return {};

        activated = multicore.coreId;
        return true;
    }

    bool KernelTask::Run() const {
        const auto process{kernel.GetCurrentProcess()};
        bool hosMainThread{};

        if (process->threads.empty()) {
            process->CreateThread();
            hosMainThread = true;
        }

        if (hosMainThread) {
            const auto thread{process->threads.front()};
            if (const auto firstThread{process->handles.GetThread(thread)}) {
                firstThread->Start();
            }
        }
        if (hosMainThread) {
            DeactivateCore();
            return {};
        }

        return true;
    }

    void KernelTask::DeactivateCore() const {
        kernel.corePid.erase(activated);
    }
    void KernelTask::DeactivateCore(CoreContainer& multicore) const {
        multicore.state = CoreState::Stopped;
        DeactivateCore();
    }
}
