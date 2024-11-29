#pragma once

#include <generic_kernel/constants.h>
#include <generic_kernel/base/kauto_object.h>
namespace Plusnx::GenericKernel::Types {
    class KSharedMemory : public Base::KAutoObject {
    public:
        KSharedMemory(Kernel& kernel);
        ~KSharedMemory();

        u8* Allocate(std::span<u8> map, const std::shared_ptr<KProcess>& required) const;

    private:
        u64 ownerProcessId;
        struct {
            i32 owner;
            i32 user;
        } permissions;

        bool allocated{};
    };
}
