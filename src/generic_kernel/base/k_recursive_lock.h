#pragma once
#include <thread>

#include <generic_kernel/kernel.h>
namespace Plusnx::GenericKernel::Types {
    class KThread;
}
namespace Plusnx::GenericKernel {
    class KRecursiveLock {
    public:
        KRecursiveLock([[maybe_unused]] Kernel& kernel) {}
        void Lock();
        void Unlock();

        std::mutex mutex;
        Types::KThread* owner{nullptr};
        i32 count{};
    };

    class KScopedLock {
    public:
        explicit KScopedLock(Kernel& kernel);
        ~KScopedLock();
    private:
        KRecursiveLock recLock;
    };

}