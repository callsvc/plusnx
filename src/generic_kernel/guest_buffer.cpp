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

    GuestBuffer::GuestBuffer(const u64 virtSize, const u64 backSize) {
        sharedFd = shm_open(MemoryBackingName, O_RDWR | O_CREAT, S_IRUSR | S_IWUSR);
        ftruncate(sharedFd, backSize);

        backing = std::span{static_cast<u8*>(mmap(nullptr, backSize, permissions, MAP_SHARED, sharedFd, 0)), backSize};
        guest = std::span{static_cast<u8*>(mmap(nullptr, virtSize, permissions, MAP_ANONYMOUS | MAP_PRIVATE | MAP_NORESERVE, -1, 0)), virtSize};

        if (const auto pointer = guest; pointer.data()) {
            if (pointer.data() != MAP_FAILED)
                madvise(pointer.data(), virtSize, MADV_HUGEPAGE);
        }

        blocks.emplace(backing.data(), KMemoryBlockInfo{});
        blocks.emplace(backing.end().base(), KMemoryBlockInfo{});
    }

    GuestBuffer::~GuestBuffer() {
        if (backing.data() != MAP_FAILED)
            if (munmap(backing.data(), backing.size()) != 0)
                return;

        if (guest.data() != MAP_FAILED)
            if (munmap(guest.data(), guest.size()) != 0)
                return;

        shm_unlink(MemoryBackingName);
    }

    // https://en.wikipedia.org/wiki/Heap_(data_structure)
    void GuestBuffer::Map(const u64 vaddr, const u64 addr, const u32 flags, u64 size, const MemoryType type) {
        size = boost::alignment::align_up(size, SwitchPageSize);

        auto [location, block] = Search(vaddr);
        if (location) {
            if (block.state != MemoryType::Free)
                std::terminate();
        }

        location = blocks.begin()->first + addr;

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

        if (assignable) {
            if (const auto allocated{static_cast<u8*>(mmap(guest.data() + vaddr, size, permissions, MAP_SHARED | MAP_FIXED, sharedFd, addr))}; allocated != MAP_FAILED)
                blocks.insert_or_assign(location, KMemoryBlockInfo(allocated, size, type, permissions));
        }

        flatmap.clear();
        Protect(vaddr, size, flags);
    }

    void GuestBuffer::Protect(const u64 vaddr, u64 size, const u32 flags) {
        size = boost::alignment::align_up(size, SwitchPageSize);
        const auto [base, block] = Search(vaddr);

        if (!block.baseAddress ||
            block.size < size ||
            block.permission == flags)
            return;

        {
            assert(blocks.contains(base));
            assert(blocks[base].baseAddress == block.baseAddress);
            assert(blocks[base].size == size);
        }

        assert(mprotect(block.baseAddress, size, flags) == 0);
        blocks[base].permission = flags;
    }

    void GuestBuffer::Allocate(const u64 vaddr, const u32 flags, const MemoryType type, const std::span<u8>& userdata) {
        if (!userdata.size() ||
            type == MemoryType::Kernel)
            return;

        Map(vaddr, vaddr, flags, userdata.size(), type);
        Protect(vaddr, userdata.size(), MemoryProtection::Read | MemoryProtection::Write);

        auto [base, block] = Search(vaddr);

        const auto source{userdata.begin().base()};
        if (block.baseAddress)
            std::memcpy(block.baseAddress, source, userdata.size());
        else
            assert(0);

        Protect(vaddr, userdata.size(), flags);
    }

    void GuestBuffer::Unmap(const u64 vaddr, u64 size) {
        auto [base, block] = Search(vaddr);

        if (!blocks.contains(base))
            return;
        size = boost::alignment::align_up(size, SwitchPageSize);

        if (const auto desc = blocks.lower_bound(base); desc != blocks.end()) {
            if (desc->second.size == size)
                desc->second.state = MemoryType::Free;
        }

        assert(madvise(block.baseAddress, block.size, MADV_REMOVE) == 0);
        flatmap.clear();
    }

    std::span<u8> GuestBuffer::GetGuestSpan(const u64 vaddr) {
        auto [base, block] = Search(vaddr);

        if (blocks.contains(base)) {
            const auto& [baseAddress, size, state, _] = blocks.at(base);
            assert(state != MemoryType::Free);
            if (size)
                return std::span(baseAddress, size);
        }
        return {};
    }

    const auto InsideRange = [](const auto& value, const auto& low, const auto& high) {
        return value >= low && value < high;
    };

    std::pair<u8*, KMemoryBlockInfo> GuestBuffer::Search(const u64 vaddr) {
        for (const auto& [_vaddr, base] : flatmap) {
            if (_vaddr != ClearSwitchPage(vaddr))
                continue;
            assert(blocks.contains(base));
            return std::make_pair(base, blocks[base]);
        }
        const auto target{guest.begin().base() + ClearSwitchPage(vaddr)};

        for (const auto& [address, block] : blocks) {
            if (!block.size)
                continue;

            const auto base{block.baseAddress};
            if (!InsideRange(target, base, base  + block.size))
                continue;
            flatmap.emplace_back(ClearSwitchPage(vaddr), address);
            return std::make_pair(address, block);
        }
        return {};
    }

    u64 GuestBuffer::GetUsedResourceSize() {
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
