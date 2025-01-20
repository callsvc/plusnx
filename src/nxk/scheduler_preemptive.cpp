#include <nxk/types/kthread.h>
#include <nxk/scheduler_preemptive.h>
namespace Plusnx::Nxk {
    void SchedulerPreemptive::AddThread(const std::shared_ptr<Types::KThread>& thrShared, u64 start) {
        std::scoped_lock lock(thrOwnerLock);

        assert(!thrShared->running);
        thrShared->running = true;
        cores.emplace_back(start);

        threads.emplace_back(thrShared);
    }
    std::shared_ptr<Types::KThread> SchedulerPreemptive::PreemptForCore(const u64 cpuid) const {
        if (!ContainsValue(cores, cpuid))
            return {};

        if (const auto thread{threads.front()}) {
            return thread;
        }
        return {};
    }

    void SchedulerPreemptive::RemoveThread(const std::shared_ptr<Types::KThread>& thrShared) {
        {
            std::scoped_lock lock(thrOwnerLock);
            const auto count{threads.size()};
            if (const auto thrFound{std::ranges::find(threads, thrShared)}; thrFound != threads.end())
                threads.erase(thrFound);
            if (count != threads.size())
                assert(thrShared->running);
        }
        thrShared->running = false;

        cores.pop_back();
    }

    bool SchedulerPreemptive::IsCoreEnabled(const u64 isEnabled) const {
        return ContainsValue(cores, isEnabled);
    }
}
