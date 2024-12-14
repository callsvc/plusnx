#include <nxk/types/kthread.h>

namespace Plusnx::Nxk::Types {
    void KThread::Initialize(u8* entry, [[maybe_unused]] u8* tls, u8* topstack) {
        stack = topstack;

        threadId = kernel.CreateThreadId();

        this->entry = entry;
    }

    void KThread::Start() {
        pthread_getname_np(pthread_self(), oldName.data(), oldName.size());

        const auto selfName{std::format("KThread {}", threadId)};
        pthread_setname_np(pthread_self(), selfName.c_str());
    }

    void KThread::Stop() const {
        pthread_setname_np(pthread_self(), oldName.data());
    }
}
