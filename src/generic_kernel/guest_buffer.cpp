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
    GuestBuffer::GuestBuffer(const u64 virtSize, const u64 backSize) {
        constexpr auto flags{MemoryProtection::Read | MemoryProtection::Write};
        resource = shm_open(MemoryBackingName, O_RDWR | O_CREAT, S_IRUSR | S_IWUSR);
        ftruncate(resource, backSize);

        backing = std::span{static_cast<u8*>(mmap(nullptr, backSize, flags, MAP_SHARED, resource, 0)), backSize};
        guest = std::span{static_cast<u8*>(mmap(nullptr, virtSize, flags, MAP_ANONYMOUS | MAP_PRIVATE | MAP_NORESERVE, -1, 0)), virtSize};

        if (const auto pointer = guest; pointer.data()) {
            if (pointer.data() != MAP_FAILED)
                madvise(pointer.data(), virtSize, MADV_HUGEPAGE);
        }

        descriptor.emplace(backing.data(), MapReserve{MemoryType::Free});
        descriptor.emplace(backing.end().base(), MapReserve{MemoryType::Free});
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
            if (block.type != MemoryType::Free)
                std::terminate();
        }

        location = descriptor.begin()->first + addr;

        auto lower{descriptor.lower_bound(location)};
        auto upper{descriptor.upper_bound(location)};

        bool assignable{false};
        if (lower->first == upper->first)
            --lower;

        if (lower->first + size < upper->first) {
            assignable = true;
        } else if (upper = descriptor.lower_bound(lower->first + size); --upper != lower) {
            if (upper->second.type == MemoryType::Free)
                descriptor.erase(upper);
            assignable = true;
        }

        if (assignable) {
            if (const auto allocated{static_cast<u8*>(mmap(guest.data() + vaddr, size, PROT_READ | PROT_WRITE, MAP_SHARED | MAP_FIXED, resource, addr))}; allocated != MAP_FAILED)
                descriptor.insert_or_assign(location, MapReserve(type, allocated, size));
        }

        flatmap.clear();
        Protect(vaddr, size, flags);
    }

    void GuestBuffer::Protect(const u64 vaddr, u64 size, const u32 flags) {
        size = boost::alignment::align_up(size, SwitchPageSize);
        const auto [base, block] = Search(vaddr);

        if (!block.user || block.size < size)
            return;

        assert(descriptor.contains(base));
        assert(descriptor[base].size == size);

        assert(mprotect(block.user, size, flags) == 0);
    }

    void GuestBuffer::Allocate(const u64 vaddr, const u32 flags, const MemoryType type, const std::span<u8>& userdata) {
        if (!userdata.size() ||
            type == MemoryType::Kernel)
            return;

        Map(vaddr, vaddr, flags, userdata.size(), type);
        Protect(vaddr, userdata.size(), MemoryProtection::Read | MemoryProtection::Write);

        auto [base, block] = Search(vaddr);

        const auto source{userdata.begin().base()};
        if (block.user)
            std::memcpy(block.user, source, userdata.size());
        Protect(vaddr, userdata.size(), flags);
    }

    void GuestBuffer::Unmap(const u64 vaddr, u64 size) {
        auto [base, block] = Search(vaddr);

        if (!descriptor.contains(base))
            return;
        size = boost::alignment::align_up(size, SwitchPageSize);

        if (const auto desc = descriptor.lower_bound(base); desc != descriptor.end()) {
            if (desc->second.size == size)
                desc->second.type = MemoryType::Free;
        }

        assert(madvise(block.user, block.size, MADV_REMOVE) == 0);
        flatmap.clear();
    }

    std::span<u8> GuestBuffer::GetGuestSpan(const u64 vaddr) {
        auto [base, block] = Search(vaddr);

        if (descriptor.contains(base)) {
            const auto& [type, user, size] = descriptor.at(base);
            if (size)
                return std::span(user, size);
        }
        return {};
    }

    const auto InsideRange = [](const auto& value, const auto& low, const auto& high) {
        return value >= low && value < high;
    };

    std::pair<u8*, MapReserve> GuestBuffer::Search(const u64 vaddr) {
        for (const auto& [_vaddr, base] : flatmap) {
            if (_vaddr != ClearSwitchPage(vaddr))
                continue;
            assert(descriptor.contains(base));
            return std::make_pair(base, descriptor[base]);
        }

        const auto target{guest.begin().base() + ClearSwitchPage(vaddr)};

        for (const auto& [back, block] : descriptor) {
            if (!block.size)
                continue;

            if (InsideRange(target, block.user, block.user + block.size)) {
                flatmap.emplace_back(ClearSwitchPage(vaddr), back);
                return std::make_pair(back, block);
            }
        }
        return {};
    }

    u64 GuestBuffer::GetUsedResourceSize() {
        u64 used{};
        u64 claimed{};
        for (const auto& block : std::ranges::views::values(descriptor)) {
            if (block.type != MemoryType::Free)
                used += block.size;
            else
                claimed += block.size;
        }
        return used + claimed;
    }
}
