#include <generic_kernel/base/k_recursive_lock.h>
namespace Plusnx::GenericKernel::Base {
}

void Plusnx::GenericKernel::KRecursiveLock::Lock() {
    mutex.lock();
    count++;
}

void Plusnx::GenericKernel::KRecursiveLock::Unlock() {
    if (!--count)
        mutex.unlock();
}

Plusnx::GenericKernel::KScopedLock::KScopedLock(Kernel& kernel) : recLock(kernel) {
    recLock.Lock();
}
Plusnx::GenericKernel::KScopedLock::~KScopedLock() {
    recLock.Unlock();
}
