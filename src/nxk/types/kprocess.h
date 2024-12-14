#pragma once

#include <nxk/types/kthread.h>

#include <nxk/user_space.h>
#include <sys_fs/meta_program.h>

#include <nxk/base/auto_types.h>
#include <nxk/base/k_tls_page.h>
#include <nxk/base/k_process_handle_table.h>
#include <nxk/svc/svc_types.h>

namespace Plusnx::Nxk::Types {
    class KProcess final : public Base::KSynchronizationObject {
    public:
        explicit KProcess(Kernel& kernel);
        void Initialize();
        void Destroy();

        void SetProgramImage(u64& vaddr, std::array<std::span<u8>, 3> sections, const std::vector<u8>& program, bool allocate) const;
        void AllocateTlsHeapRegion();

        void CreateThread();

        void* entry{nullptr};
        KThread* eThread{nullptr};
        std::list<std::unique_ptr<KTlsPage>> partialTlsSlots;
        std::list<std::unique_ptr<KTlsPage>> fullTlsPages;

        u64 pid{};
        void* exceptionTlsArea{nullptr};

        std::optional<Svc::CreateProcessParameter> creation{};

        Base::KProcessHandleTable handles;
        std::list<u16> threads;

        // https://medium.com/@boutnaru/linux-kernel-mm-struct-fafe50b57837
        std::unique_ptr<UserSpace>& mm;
        SysFs::MetaProgram npdm{};

    private:
        std::mutex threadsLock;
    };
}