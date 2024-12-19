#include <print>
#include <chrono>

#include <nxk/kernel.h>
#include <cpu/core_blob.h>
namespace Plusnx::Cpu {
    using namespace std::chrono_literals;

    bool MigrateThreadToCore(const u8 core) {
        cpu_set_t enabled;
        if (pthread_getaffinity_np(pthread_self(), sizeof(enabled), &enabled))
            return {};
        if (!CPU_ISSET(core, &enabled)) {
            return {};
        }

        cpu_set_t target;
        CPU_ZERO(&target);
        CPU_SET(core, &target);

        return pthread_setaffinity_np(pthread_self(), sizeof(target), &target) == 0;
    }

    void CoreBlob::RunThread(const std::stop_token& stop) {
        const auto fromCore{sched_getcpu()};
        std::println("New thread created on core: {}", fromCore);

        assert(MigrateThreadToCore(cpusched));
        assert(sched_getcpu() == static_cast<i32>(cpusched));
        std::println("Thread migration complete, from core {} to {}", fromCore, cpusched);

        coreTask.emplace(*this, kernel);
        assert(stop.stop_possible());
        while (true) {
            if (stop.stop_requested()) {
                break;
            }
            if (coreTask->CheckForActivation()) {
                if (!coreTask->PreemptAndRun())
                    break;
            }
        }

        coreTask->DeactivateCore();
    }

    void CoreBlob::Initialize() {
        assert(kernel.corePid.empty());
        thread.emplace([&] (const std::stop_token& stop) {
            RunThread(stop);
        });
    }

    void CoreBlob::Destroy() {
        Enabled(false);
        thread->request_stop();

        thread->join();
    }
}
