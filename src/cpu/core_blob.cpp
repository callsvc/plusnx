#include <print>
#include <chrono>

#include <nxk/kernel.h>
#include <cpu/core_blob.h>
namespace Plusnx::Cpu {
    using namespace std::chrono_literals;

    void CoreBlob::RunThread(const std::stop_token& stop) {
        cpuid = sched_getcpu();
        coreTask.emplace(kernel, cpuid);

        assert(stop.stop_possible());
        std::println("New thread created on core: {}", cpuid);

        while (true) {
            if (stop.stop_requested()) {
                return;
            }
            state = CoreState::Waiting;
            if (coreTask->CheckForActivation(*this)) {
                if (!coreTask->PreemptAndRun())
                    break;
            }
        }

        coreTask->DeactivateCore(*this);
    }

    void CoreBlob::Initialize() {
        assert(kernel.corePid.empty());
        thread.emplace([&] (const std::stop_token& stop) {
            RunThread(stop);
        });
    }

    void CoreBlob::Destroy() {
        state = CoreState::Stopped;
        state.notify_one();
        thread->request_stop();

        thread->join();
    }
}
