#include <print>
#include <chrono>

#include <cpu/core_container.h>
namespace Plusnx::Cpu {
    using namespace std::chrono_literals;

    void CoreContainer::RunThread(const std::stop_token& stop) {
        coreId = sched_getcpu();
        assert(stop.stop_possible());

        std::print("New thread created on core: {}\n", coreId);

        while (true) {
            if (stop.stop_requested())
                return;
            std::this_thread::sleep_for(4s);
        }
    }

    void CoreContainer::Initialize() {
        thread.emplace([&] (const std::stop_token& stop) {
            RunThread(stop);
        });
    }

    void CoreContainer::Destroy() {
        thread->request_stop();
    }
}
