#pragma once
#include <list>

#include <gsk/kernel.h>
#include <gsk/types/kthread.h>

namespace Plusnx::Gsk::Types {
    class KProcess : KBaseType {
    public:
        explicit KProcess(Kernel& kernel) : KBaseType(kernel, KType::KProcess) {}
        void Initialize();
        void Destroy();

        void* entry{nullptr};
        u64 pid{};

        std::list<KThread> threads;
        KThread* eThread{nullptr};
    };
}