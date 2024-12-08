#pragma once
#include <array>

#include <generic_kernel/kernel.h>
namespace Plusnx::GenericKernel {
    class KTlsPage {
    public:
        KTlsPage(Kernel& kernel);
        ~KTlsPage();
        bool HasAvailableSlots() const;
        u8* AllocateSlot();
        void FreeSlot(u8* slot);

        u8* tls{nullptr};
        std::array<bool, 8> freeSlots;
        Kernel& kernel;
    };
}