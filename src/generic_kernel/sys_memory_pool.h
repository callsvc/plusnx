#pragma once
#include <boost/container/flat_map.hpp>
#include <generic_kernel/constants.h>
namespace Plusnx::GenericKernel {
    struct Pool {
        void* pointer;
        u64 size;
    };

    enum class MemoryType : u32 {
        Free,
        Code = 0x00DC7E03, // Mapped during #CreateProcess
        Alias = 0x00482907,
        Kernel = 0x00002013,
        Stack = 0x005C3C0B,
    };
    struct VAddrPool {
        MemoryType type;
        u64 count; // Number of pages associated with this pool (size = count * SwitchPageSize)
    };

    class SysMemoryPool {
    public:
        SysMemoryPool(u64 virtualSize, u64 backingSize);
        ~SysMemoryPool();

        void MapPages(u64 vaddr, u64 addr, u32 flags, u64& size) const;
        void Map(u64 vaddr, u64 addr, u32 flags, u64 size);
        void CopyUserMemory(u64 vaddr, u32 flags, MemoryType type, const std::span<u8>& userData);

        void Unmap(u64 vaddr, u64 size);

        u8* GetPointer(u64 vaddr) const;
    private:
        uintptr_t PickMemoryAddr(const u64 vaddr, const u32 vm = 1) const {
            return reinterpret_cast<uintptr_t>(pools.at(vm).pointer) + vaddr;
        }

        std::vector<Pool> pools;
        // https://www.boost.org/doc/libs/1_62_0/doc/html/container/non_standard_containers.html#container.non_standard_containers.flat_xxx
        boost::container::flat_map<uintptr_t, VAddrPool> descriptor;
        i32 mfd;
    };
}