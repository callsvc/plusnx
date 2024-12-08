#include <generic_kernel/memory/k_slab_heap.h>
#include <generic_kernel/base/k_tls_page.h>
namespace Plusnx::GenericKernel {
    constexpr auto TlsEntrySize{0x200};

    KTlsPage::KTlsPage(Kernel& kernel) : kernel(kernel) {
        assert(sizeof(freeSlots) == SwitchPageSize / TlsEntrySize);
        tls = kernel.slabHeap->Allocate();
        freeSlots.fill(true);
    }

    KTlsPage::~KTlsPage() {
        kernel.slabHeap->Free(tls);
    }

    bool KTlsPage::HasAvailableSlots() const {
        bool available{};
        for (const auto& slot : freeSlots)
            available |= slot;

        return available;
    }

    u8* KTlsPage::AllocateSlot() {
        auto* slot = [&] -> u8* {
            auto slotPtr{tls};
            for (auto& free : freeSlots) {
                if (free) {
                    free = {};
                    return slotPtr;
                }
                slotPtr += TlsEntrySize;
            }
            return {};
        }();
        return slot;
    }

    void KTlsPage::FreeSlot(u8* slot) {
        auto slotPtr{slot};
        for (auto& free : freeSlots) {
            if (slotPtr == slot)
                free = true;
            slotPtr += TlsEntrySize;
        }
    }
}
