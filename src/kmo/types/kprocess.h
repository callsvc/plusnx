#pragma once
#include <list>

#include <kmo/kernel.h>
#include <kmo/types/kthread.h>

namespace Plusnx::Kmo::Types {
    class KProcess : KBaseType {
    public:
        KProcess(Kernel& kernel) : KBaseType(kernel, KType::KProcess) {}
        void Initialize();
        void Destroy();

        void* entry{nullptr};
        u64 pid{};

        std::list<KThread> threads;
        KThread* eThread{nullptr};
    };
}