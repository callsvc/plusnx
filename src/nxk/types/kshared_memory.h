#pragma once

#include <nxk/constants.h>
#include <nxk/base/k_auto_object.h>
#include <nxk/types/kprocess.h>
namespace Plusnx::Nxk::Types {
    class KSharedMemory final : public Base::KAutoObject {
    public:
        KSharedMemory(Kernel& kernel);

        u8* Allocate(u8* vaddr, u64 size, const std::shared_ptr<KProcess>& process) const;
        void Free(u8* vaddr, u64 size, const std::shared_ptr<KProcess>& process) const;

    private:
        u64 ownerProcessId;
        struct {
            i32 owner;
            i32 user;
        } permissions;

        bool allocated{};
    };
}
