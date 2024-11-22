#include <sys/mman.h>
#include <fcntl.h>
#include <print>
#include <boost/align/align_up.hpp>
#include <generic_kernel/sys_memory_pool.h>
namespace Plusnx::GenericKernel {
    constexpr auto SharedMemory{"/PlusnxBackingMemory"};

    SysMemoryPool::SysMemoryPool(const u64 virtualSize, const u64 backingSize) {
        constexpr auto flags{MemoryProtection::Read | MemoryProtection::Write};

        mfd = shm_open(SharedMemory, O_RDWR | O_CREAT, S_IRUSR | S_IWUSR);
        ftruncate(mfd, backingSize);

        pools.emplace_back(mmap(nullptr, backingSize, flags, MAP_SHARED, mfd, 0), backingSize);
        pools.emplace_back(mmap(nullptr, virtualSize, flags, MAP_ANONYMOUS | MAP_PRIVATE | MAP_NORESERVE, -1, 0), virtualSize);

        if (const auto [pointer, size] = pools.front(); pointer) {
            if (pointer != MAP_FAILED)
                madvise(pointer, size, MADV_HUGEPAGE);
        }
    }

    SysMemoryPool::~SysMemoryPool() {
        for (const auto [pointer, size] : pools) {
            if (munmap(pointer, size) != 0) {
                std::print("{}\n", GetOsErrorString());
            }
        }
        shm_unlink(SharedMemory);
    }
    // https://en.wikipedia.org/wiki/Heap_(data_structure)
    void SysMemoryPool::MapPages(const u64 vaddr, const u64 addr, const u32 flags, u64& size) const {
        size = boost::alignment::align_up(size, SwitchPageSize);
        if (mmap(reinterpret_cast<void*>(vaddr), size, flags, MAP_SHARED | MAP_FIXED, mfd, addr) == MAP_FAILED)
            throw runtime_plusnx_except("Memory mapping failed, cause: {}", GetOsErrorString());
    }

    void SysMemoryPool::Map(const u64 vaddr, const u64 addr, const u32 flags, u64 size) {
        const auto address{PickMemoryAddr(vaddr)};
        MapPages(address, addr, flags, size);
        descriptor.emplace(address, VAddrPool(MemoryType::Alias, size / SwitchPageSize));
    }

    void SysMemoryPool::CopyUserMemory(const u64 vaddr, const u32 flags, const MemoryType type, const std::span<u8>& userData) {
        if (!userData.size())
            return;
        const auto address{PickMemoryAddr(vaddr)};
        auto size{userData.size()};
        MapPages(address, vaddr, flags, size);

        assert(mprotect(reinterpret_cast<void*>(address), size, PROT_READ | PROT_WRITE) == 0);

        std::memcpy(reinterpret_cast<void*>(address), userData.data(), userData.size());
        assert(mprotect(reinterpret_cast<void*>(address), size, flags) == 0);

        descriptor.emplace(address, VAddrPool(type, size / SwitchPageSize));
    }

    void SysMemoryPool::Unmap(const u64 vaddr, u64 size) {
        const auto address{PickMemoryAddr(vaddr)};

        if (!descriptor.contains(address))
            return;
        size = boost::alignment::align_up(size, SwitchPageSize);
        const auto pages{size / SwitchPageSize};

        if (const auto desc = descriptor.lower_bound(address); desc != descriptor.end()) {
            if (desc->second.count == pages)
                descriptor.erase(desc);
            else
                desc->second.count -= pages;
        }

        assert(madvise(reinterpret_cast<void*>(address), size, MADV_REMOVE) == 0);
    }

    u8* SysMemoryPool::GetPointer(const u64 vaddr) const {
        const auto target{PickMemoryAddr(ClearPage(vaddr))};
        if (descriptor.contains(target)) {
            return reinterpret_cast<u8*>(target);
        }
        return {};
    }
}
