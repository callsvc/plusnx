#pragma once
#include <list>

#include <generic_kernel/kernel.h>
#include <generic_kernel/types/kthread.h>

#include <generic_kernel/user_space.h>
#include <sys_fs/npdm.h>

namespace Plusnx::GenericKernel::Types {
    class KProcess : public KBaseType {
    public:
        explicit KProcess(Kernel& kernel);
        void Initialize();
        void Destroy();

        void* entry{nullptr};
        KThread* eThread{nullptr};

        u64 pid{};

        std::list<KThread> threads;
        std::unique_ptr<UserSpace>& us;
        SysFs::Npdm npdm{};
    };
}