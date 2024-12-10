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

    class CoreContainer {
    public:
        CoreContainer(GenericKernel::Kernel& creator) : kernel(creator) {}
        void RunThread(const std::stop_token& stop);

        void Initialize();

        void Destroy();

        std::atomic<CoreState> state;
        u64 coreId{};
    private:
        std::optional<std::jthread> thread;
        std::optional<KernelTask> dealer;

        GenericKernel::Kernel& kernel;
    };
}