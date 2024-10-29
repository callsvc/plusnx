#include <generic_kernel/types/kthread.h>

namespace Plusnx::GenericKernel::Types {
    void KThread::Initialize() {
        const auto selfName{std::format("KThread.{}", threadId)};
        pthread_setname_np(pthread_self(), selfName.c_str());
    }
}
