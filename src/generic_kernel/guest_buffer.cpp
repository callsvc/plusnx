#include <mutex>
#include <print>
#include <ranges>

#include <fcntl.h>
#include <fmt/format.h>
#include <boost/align/align_up.hpp>

#include <sys_fs/fs_types.h>
#include <generic_kernel/guest_buffer.h>
namespace Plusnx::GenericKernel {

    constexpr auto MemoryBackingName{"BackingMemory"};
    constexpr auto permissions{MemoryProtection::Read | MemoryProtection::Write};

    GuestBuffer::~GuestBuffer() {
        if (backing.data() != MAP_FAILED)
            if (munmap(backing.data(), backing.size()) != 0)
                return;

        if (guest.data() != MAP_FAILED)
            if (munmap(guest.data(), guest.size()) != 0)
                return;

        shm_unlink(MemoryBackingName);
    }

    void GuestBuffer::Initialize(const u64 virtSize, const u64 backSize) {
        sharedFd = shm_open(MemoryBackingName, O_RDWR | O_CREAT, S_IRUSR | S_IWUSR);
        ftruncate(sharedFd, backSize);

        backing = std::span{static_cast<u8*>(mmap(nullptr, backSize, permissions, MAP_SHARED, sharedFd, 0)), backSize};
        guest = std::span{static_cast<u8*>(mmap(nullptr, virtSize, permissions, MAP_ANONYMOUS | MAP_PRIVATE | MAP_NORESERVE, -1, 0)), virtSize};

        if (const auto pointer = guest; pointer.data()) {
            if (pointer.data() != MAP_FAILED)
                madvise(pointer.data(), virtSize, MADV_HUGEPAGE);
        }

        blocks.emplace(guest.data(), KMemoryBlockInfo{});
        blocks.emplace(guest.end().base(), KMemoryBlockInfo{});
    }

    u8* GuestBuffer::Allocate(u8* vaddr, u8* addr, const u32 flags, const u64 size, const MemoryType type) {
        std::scoped_lock guard(lock);

        return Map(vaddr, addr, flags, size, type);
    }

    void GuestBuffer::Free(u8* address, const u64 size) {
        std::scoped_lock guard(lock);
        Unmap(address, size);
    }

    u8* GuestBuffer::Map(u8* vaddr, u8* addr, const u32 flags, u64 size, const MemoryType type) {
        size = boost::alignment::align_up(size, SwitchPageSize);

        auto [location, block] = Search(vaddr);
        if (location) {
            if (block->state != MemoryType::Free) {
            }
        } else {
            location = vaddr;
        }

        auto lower{blocks.lower_bound(location)};
        auto upper{blocks.upper_bound(location)};

        bool assignable{false};
        if (lower->first == upper->first)
            --lower;

        if (lower->first + size < upper->first) {
            assignable = true;
        } else if (upper = blocks.lower_bound(lower->first + size); --upper != lower) {
            if (upper->second.state == MemoryType::Free)
                blocks.erase(upper);
            assignable = true;
        }

        u8* allocated{};
        if (assignable) {
            allocated = static_cast<u8*>(mmap(vaddr, size, permissions, MAP_SHARED | MAP_FIXED, sharedFd, static_cast<u64>(addr - backing.begin().base())));
            if (allocated == MAP_FAILED) {
                return {};
            }
            blocks.insert_or_assign(allocated, KMemoryBlockInfo(addr, size, type, permissions));

            flatmap.clear();
            Protect(allocated, size, flags);
        }

        return allocated;
    }

    void GuestBuffer::Protect(u8* vaddr, u64 size, const u32 flags) {
        size = boost::alignment::align_up(size, SwitchPageSize);
        const auto [base, block] = Search(vaddr);

        if (!base || block->permission == flags)
            return;

        assert(mprotect(base, size, flags) == 0);
        block->permission = flags;
    }

    void GuestBuffer::Unmap(u8* address, u64 size) {
        if (!blocks.contains(address))
            return;
        size = boost::alignment::align_up(size, SwitchPageSize);

        if (auto [vaddr, block] = Search(address); vaddr) {
            if (block->size == size)
                block->state = MemoryType::Free;

            assert(madvise(address, block->size, MADV_REMOVE) == 0);
        }
        flatmap.clear();
    }

    std::span<u8> GuestBuffer::GetGuestSpan(u8* vaddr) {
        std::scoped_lock guard(lock);
        auto [base, block] = Search(vaddr);

        if (blocks.contains(base)) {
            const auto& [baseAddress, size, state, _] = blocks.at(base);
            assert(state != MemoryType::Free);
            if (size)
                return std::span(baseAddress, size);
        }
        return {};
    }

    std::pair<u8*, KMemoryBlockInfo*> GuestBuffer::Search(u8* vaddr) {
        for (const auto& [_vaddr, block] : flatmap) {
            if (_vaddr != vaddr)
                continue;
            assert(blocks.contains(_vaddr));
            return std::make_pair(_vaddr, block);
        }

        auto base{blocks.lower_bound(vaddr)};
        if (base == blocks.end())
            return {};

        if (base->first > vaddr)
            --base;
        if (base->first) {
            flatmap.emplace_back(base->first, &base->second);

            return std::make_pair(flatmap.back().vaddr, flatmap.back().block);
        }
        return {};
    }

    u64 GuestBuffer::GetUsedResourceSize() {
        std::scoped_lock guard(lock);

        u64 used{};
        u64 claimed{};
        for (const auto& block : std::ranges::views::values(blocks)) {
            if (block.state != MemoryType::Free)
                used += block.size;
            else
                claimed += block.size;
        }
        return used + claimed;
    }
}
