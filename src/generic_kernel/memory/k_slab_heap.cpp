#include <generic_kernel/kernel.h>
#include <generic_kernel/memory/k_slab_heap.h>
namespace Plusnx::GenericKernel::Memory {
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

        auto* tlsBase{kernel.memory->tlsIo.begin().base() + object};
        auto* backBase{kernel.nxmemory->backing.data() + object};
        kernel.nxmemory->Allocate(tlsBase, backBase, MemoryProtection::Read | MemoryProtection::Write, itemSize, MemoryType::ThreadLocal);
        std::memset(tlsBase, 0, itemSize);

        allocated.emplace(tlsBase, object);

        return tlsBase;
    }

    void KSlabHeap::Free(u8* object) {
        if (object == nullptr)
            return;
        assert(allocated.size());

        if (!allocated.contains(object))
            throw runtime_exception("This object does not belong to the current slab");
        {
            slotObjects.emplace_back(allocated[object]);
            allocated.erase(object);
        }
    }
}
