#include <nxk/kernel.h>
#include <nxk/memory/k_slab_heap.h>
namespace Plusnx::Nxk::Memory {
    KSlabHeap::KSlabHeap(Kernel& kernel, u64 vbase, const u64 object, const u64 size) : slotBase(vbase), slotSize(size), itemSize(object), kernel(kernel) {
        assert(size > 0);
        const auto slabEnd{vbase + size};

        while (vbase != slabEnd) {
            slotObjects.emplace_back(vbase);
            vbase += object;
        }
        const auto listCount{size / object};
        assert(slotObjects.size() == listCount);

    }

    u8* KSlabHeap::Allocate() {
        if (slotObjects.empty()) {
            return {};
        }
        const auto object{slotObjects.front()};
        slotObjects.pop_front();

        auto* tlsBase{kernel.user->tlsIo.begin().base() + object};
        auto* backBase{kernel.nxmemory->back->data() + object};
        static bool tlsReserved{false};
        [[unlikely]] if (!tlsReserved) {
            kernel.nxmemory->Reserve(tlsBase, backBase, itemSize * slotObjects.size() + 1);
            tlsReserved = true;
        }

        kernel.nxmemory->Allocate(tlsBase, backBase, itemSize, MemoryType::ThreadLocal);
        std::memset(tlsBase, 0, itemSize);

        allocated.emplace(tlsBase, object);

        return tlsBase;
    }

    void KSlabHeap::Free(u8* object) {
        if (object == nullptr)
            return;
        assert(allocated.size());

        if (!allocated.contains(object))
            throw exception("This object does not belong to the current slab");
        {
            slotObjects.emplace_back(allocated[object]);
            allocated.erase(object);
        }
    }
}
