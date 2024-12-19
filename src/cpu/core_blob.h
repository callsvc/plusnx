#pragma once
#include <thread>

#include <types.h>

#include <cpu/kernel_task.h>
#include <cpu/unit_types.h>
namespace Plusnx::Cpu {
    class CoreBlob : public TaskableCoreContext {
    public:
        CoreBlob(const u64 coreId, Nxk::Kernel& _kernel) : TaskableCoreContext(coreId), kernel(_kernel) {}

        void RunThread(const std::stop_token& stop);

        void Initialize();

        void Destroy();
    private:
        std::optional<std::jthread> thread;
        std::optional<KernelTask> coreTask;

        Nxk::Kernel& kernel;
    };
}