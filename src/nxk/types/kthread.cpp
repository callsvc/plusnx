#include <nxk/types/kthread.h>

namespace Plusnx::Nxk::Types {
    void KThread::Initialize(u8* _entry, u8* tls, u8* topstack) {
        stack = topstack;

        threadId = kernel.CreateThreadId();
        entry = _entry;
        threadTls = tls;

        pthread_getname_np(pthread_self(), oldName.data(), oldName.size());
        const auto selfName{std::format("KThread {}", threadId)};
        pthread_setname_np(pthread_self(), selfName.c_str());

        assert(!running);
    }

    void KThread::Run() const {
        *threadTls = {};
        std::this_thread::sleep_for(2s);
    }

    void KThread::Kill() {
        pthread_setname_np(pthread_self(), oldName.data());
        threadId = {};
    }
}
