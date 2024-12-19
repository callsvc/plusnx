#pragma once
#include <thread>

#include <types.h>

#include <cpu/kernel_task.h>
#include <cpu/unit_types.h>
namespace Plusnx::Cpu {
    enum class CoreState {
        Waiting,
        Running,
        Stopped,
    };

    class CoreBlob {
    public:
        CoreBlob(Nxk::Kernel& _kernel) : kernel(_kernel) {}
        void RunThread(const std::stop_token& stop);

        void Initialize();

        void Destroy();

        std::atomic<CoreState> state;
        u64 cpuid{};
    private:
        std::optional<std::jthread> thread;
        std::optional<KernelTask> coreTask;

        Nxk::Kernel& kernel;
    };
}