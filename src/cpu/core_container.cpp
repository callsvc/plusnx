#include <print>
#include <chrono>

#include <cpu/core_container.h>

#include <nxk/kernel.h>

namespace Plusnx::Cpu {
    using namespace std::chrono_literals;

    void CoreContainer::RunThread(const std::stop_token& stop) {
        coreId = sched_getcpu();
        assert(stop.stop_possible());

        std::print("New thread created on core: {}\n", coreId);

        while (true) {
            if (stop.stop_requested())
                return;
            state = CoreState::Waiting;

            if (dealer->CheckForActivation(*this))
                if (!dealer->Run())
                    break;

            std::this_thread::sleep_for(1s);
        }
        dealer->DeactivateCore(*this);
    }

    void CoreContainer::Initialize() {
        assert(kernel.corePid.empty());
        dealer.emplace(kernel);

        thread.emplace([&] (const std::stop_token& stop) {
            RunThread(stop);
        });
    }

    void CoreContainer::Destroy() {
        state = CoreState::Stopped;
        state.notify_one();
        thread->request_stop();

        thread->join();
    }
}
