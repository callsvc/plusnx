#pragma once

#include <list>
#include <tsl/robin_map.h>

#include <types.h>
namespace Plusnx::Nxk {
    class Kernel;
}
namespace Plusnx::Nxk::Memory {
    class KSlabHeap {
    public:
        KSlabHeap(Kernel& kernel, u64 vbase, u64 object, u64 size);
        u8* Allocate();
        void Free(u8* object);

        u64 slotBase{};
        u64 slotSize{};
        u64 itemSize{};
    private:
        Kernel& kernel;
        std::list<u64> slotObjects;
        tsl::robin_map<u8*, u64> allocated;
    };
}