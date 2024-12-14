#include <nxk/base/k_recursive_lock.h>
namespace Plusnx::Nxk::Base {
}

void Plusnx::Nxk::KRecursiveLock::Lock() {
    mutex.lock();
    count++;
}

void Plusnx::Nxk::KRecursiveLock::Unlock() {
    if (!--count)
        mutex.unlock();
}

Plusnx::Nxk::KScopedLock::KScopedLock(Kernel& kernel) : recLock(kernel) {
    recLock.Lock();
}
Plusnx::Nxk::KScopedLock::~KScopedLock() {
    recLock.Unlock();
}
