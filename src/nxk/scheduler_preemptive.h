#pragma once
#include <list>
#include <memory>
#include <mutex>

namespace Plusnx::Nxk {
    namespace Types {
        class KThread;
    }
    class Kernel;
    class SchedulerPreemptive {
    public:
        SchedulerPreemptive(Kernel& _kernel) : kernel{_kernel} {}
        void AddThread(const std::shared_ptr<Types::KThread>& thrShared, u64 start);
        void RemoveThread(const std::shared_ptr<Types::KThread>& thrShared);

        bool IsCoreEnabled(u64 isEnabled) const;
        std::shared_ptr<Types::KThread> PreemptForCore(u64 cpuid) const;

    private:
        std::mutex thrOwnerLock;
        std::list<std::shared_ptr<Types::KThread>> threads;
        std::vector<u64> cores;

        Kernel& kernel;
    };
}
