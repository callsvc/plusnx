#pragma once
#include <list>

#include <generic_kernel/kernel.h>
#include <generic_kernel/types/kthread.h>

#include <generic_kernel/user_space.h>
#include <sys_fs/meta_program.h>

namespace Plusnx::GenericKernel::Types {
    class KProcess : public KBaseType {
    public:
        explicit KProcess(Kernel& kernel);
        void Initialize();
        void Destroy();

        void SetProgramImage(u64& baseAddr, std::array<std::span<u8>, 3> sections, std::vector<u8>&& program) const;

        void* entry{nullptr};
        KThread* eThread{nullptr};

        u64 pid{};

        std::list<KThread> threads;
        std::unique_ptr<UserSpace>& ptb;
        SysFs::MetaProgram npdm{};
    };
}