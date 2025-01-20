#pragma once

#include <condition_variable>
#include <types.h>
namespace Plusnx::Nxk {
    class Kernel;
}
namespace Plusnx::Cpu {
    class TaskableCoreContext {
    public:
        TaskableCoreContext(const u64 _id) : cpusched(_id) {}
        void Enabled(bool enable);
        void WaitForAvailability();

        u64 cpusched{};
    private:
        std::mutex lock;
        std::condition_variable barrier;
        bool running{};
    };

    class KernelTask {
    public:
        KernelTask(TaskableCoreContext& ctx, Nxk::Kernel& _kernel) : kernel(_kernel), context(ctx) {}

        bool CheckForActivation() const;
        void DeactivateCore();
        bool PreemptAndRun();

        bool inserted{};
    private:
        Nxk::Kernel& kernel;
        TaskableCoreContext& context;
    };
}
