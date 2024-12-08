#pragma once
#include <list>

#include <generic_kernel/types/kthread.h>

#include <generic_kernel/user_space.h>
#include <sys_fs/meta_program.h>

#include <generic_kernel/base/auto_types.h>
#include <generic_kernel/base/k_tls_page.h>
#include <generic_kernel/svc/svc_types.h>

namespace Plusnx::GenericKernel::Types {
    class KProcess : public Base::KSynchronizationObject {
    public:
        explicit KProcess(Kernel& kernel);
        void Initialize();
        void Destroy();

        void SetProgramImage(u64& vaddr, std::array<std::span<u8>, 3> sections, const std::vector<u8>& program, bool allocate) const;
        void AllocateTlsHeapRegion();

        void* entry{nullptr};
        KThread* eThread{nullptr};
        std::list<std::unique_ptr<KTlsPage>> partialTlsSlots;
        std::list<std::unique_ptr<KTlsPage>> fullTlsPages;

        u64 pid{};
        void* exceptionTlsArea{nullptr};

        std::optional<Svc::CreateProcessParameter> creation{};

        std::list<KThread> threads;
        // https://medium.com/@boutnaru/linux-kernel-mm-struct-fafe50b57837
        std::unique_ptr<UserSpace>& mm;
        SysFs::MetaProgram npdm{};
    };
}