#include <nxk/kernel.h>
#include <nxk/types/kprocess.h>

#include <cpu/core_blob.h>
#include <cpu/kernel_task.h>
namespace Plusnx::Cpu {

    void TaskableCoreContext::WaitForAvailability() {
        std::unique_lock guard(lock);

        if (!running) {
            barrier.wait(guard);
        }
        waiting = false;
        running = true;
    }
    void TaskableCoreContext::Enabled(const bool enable) {
        std::scoped_lock guard(lock);

        if (!enable) {
            running = false;
            waiting = true;
        }
        barrier.notify_one();
    }

    bool KernelTask::CheckForActivation() const {
        context.WaitForAvailability();

        if (!kernel.corePid.contains(context.cpusched))
            return {};

        return true;
    }

    bool KernelTask::PreemptAndRun() {
        const auto process{kernel.GetCurrentProcess()};
        bool hosMainThread{};

        if (inserted && !kernel.scheduler->IsCoreEnabled(context.cpusched)) {
            return {};
        }

        [[unlikely]] if (process->threads.empty()) {
            process->CreateThread();
            hosMainThread = true;
        }

        if (const auto thread{kernel.scheduler->PreemptForCore(context.cpusched)}; !hosMainThread) {
            thread->Run(); // This is our starting point
            kernel.scheduler->RemoveThread(thread); // Let's remove it just to break the application, since we don't have much to execute for now
            inserted = false;
        } else {
            const auto hos{process->threads.front()};
            if (const auto firstThread{process->handles.GetThread(hos)}) {
                kernel.scheduler->AddThread(firstThread, context.cpusched);
                inserted = true;
            }
        }
        return inserted; // Revalidate the current core again
    }

    void KernelTask::DeactivateCore() {
        const auto process{kernel.GetCurrentProcess()};
        for (const auto aliveThr : process->threads) {
            if (auto thread{process->handles.GetThread(aliveThr)}) {
                thread->Kill();
                kernel.scheduler->RemoveThread(thread);
            }
        }
        context.Enabled(false);
        kernel.corePid.erase(context.cpusched);

        inserted = false;
    }
}
