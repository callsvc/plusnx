#include <nxk/kernel.h>
#include <nxk/types/kprocess.h>

#include <cpu/core_blob.h>
#include <cpu/kernel_task.h>
namespace Plusnx::Cpu {
    bool KernelTask::CheckForActivation(const CoreBlob& multicore) const {
        if (multicore.state != CoreState::Running)
            multicore.state.wait(CoreState::Waiting);

        if (!kernel.corePid.contains(multicore.cpuid))
            return false;

        return true;
    }

    bool KernelTask::PreemptAndRun() const {
        const auto process{kernel.GetCurrentProcess()};
        bool hosMainThread{};

        [[unlikely]] if (process->threads.empty()) {
            process->CreateThread();
            hosMainThread = true;
        }

        auto RunTaskLoop = [&] {
            if (!kernel.scheduler->IsCoreEnabled(cpusched)) {
                DeactivateCore();
                return;
            }
            if (const auto thread{kernel.scheduler->PreemptForCore(cpusched)}) {
                thread->Run(); // This is our starting point
                kernel.scheduler->RemoveThread(thread); // Let's remove it just to break the application, since we don't have much to execute for now
            } else {
                DeactivateCore();
            }
        };

        [[unlikely]] if (hosMainThread) {
            const auto thread{process->threads.front()};
            if (const auto firstThread{process->handles.GetThread(thread)}) {
                kernel.scheduler->AddThread(firstThread, cpusched);
                RunTaskLoop();
                RunTaskLoop(); // We'll be destroyed before the loop executes again
            }
        } else {
            RunTaskLoop();
        }
        return true; // Revalidate the current core again
    }

    void KernelTask::DeactivateCore() const {
        const auto process{kernel.GetCurrentProcess()};
        for (const auto aliveThr : process->threads) {
            if (auto thread{process->handles.GetThread(aliveThr)}) {
                thread->Kill();
                kernel.scheduler->RemoveThread(thread);
            }
        }
        kernel.corePid.erase(cpusched);
    }
    void KernelTask::DeactivateCore(CoreBlob& multicore) const {
        multicore.state = CoreState::Stopped;
        DeactivateCore();
    }
}
