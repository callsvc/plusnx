#pragma once

#include <generic_kernel/constants.h>
#include <generic_kernel/base/kauto_object.h>
#include <generic_kernel/types/kprocess.h>
namespace Plusnx::GenericKernel::Types {
    class KSharedMemory : public Base::KAutoObject {
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
