#include <mutex>
#include <print>
#include <ranges>

#include <fcntl.h>
#include <sys/mman.h>
#include <boost/align/align_up.hpp>
#include <generic_kernel/guest_buffer.h>
namespace Plusnx::GenericKernel {
    constexpr auto MemoryBackingName{"BackingMemory"};

    GuestBuffer::GuestBuffer(const u64 virtSize, const u64 backSize) {
        constexpr auto flags{MemoryProtection::Read | MemoryProtection::Write};
        mfd = shm_open(MemoryBackingName, O_RDWR | O_CREAT, S_IRUSR | S_IWUSR);
        ftruncate(mfd, backSize);

        backing = std::span{static_cast<u8*>(mmap(nullptr, backSize, flags, MAP_SHARED, mfd, 0)), backSize};
        guest = std::span{static_cast<u8*>(mmap(nullptr, virtSize, flags, MAP_ANONYMOUS | MAP_PRIVATE | MAP_NORESERVE, -1, 0)), virtSize};

        if (const auto pointer = guest; pointer.data()) {
            if (pointer.data() != MAP_FAILED)
                madvise(pointer.data(), virtSize, MADV_HUGEPAGE);
        }

        descriptor.emplace(guest.data(), VAddrPool(MemoryType::Free, backing.data(), 0));
        descriptor.emplace(guest.end().base(), VAddrPool(MemoryType::Free, backing.end().base(), backSize));
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

    std::optional<std::span<u8>> GuestBuffer::VirtToGuest(const u64 vaddr, const u64 size) {
        std::span result{guest.data() + vaddr, size};
        if (const auto description = descriptor.find(result.data()); description != descriptor.end()) {
            if (size)
                if (!description->second.size || description->second.type == MemoryType::Free)
                    return {};
        }
        return result;
    }

    // https://en.wikipedia.org/wiki/Heap_(data_structure)
    void GuestBuffer::Map(const u64 vaddr, const u64 addr, const u32 flags, u64 size, const MemoryType type) {
        size = boost::alignment::align_up(size, SwitchPageSize);

        const auto guest{VirtToGuest(vaddr, 0)};

        auto lower{descriptor.lower_bound(guest->data())};
        auto upper{descriptor.upper_bound(guest->data())};

        bool assignable{false};
        if (lower->first == upper->first)
            --lower;

        if (lower->first + size < upper->first) {
            assignable = true;
        } else if (upper = descriptor.lower_bound(lower->first + size); --upper != lower) {
            assert(upper->second.type == MemoryType::Free);
            descriptor.erase(upper);
            assignable = true;
        }
        if (assignable) {
            if (const auto reflect{mmap(guest->data(), size, PROT_READ | PROT_WRITE, MAP_SHARED | MAP_FIXED, mfd, addr)}; reflect != MAP_FAILED)
                descriptor.insert_or_assign(guest->data(), VAddrPool(type, static_cast<u8*>(reflect), size));
        }
        Protect(vaddr, size, flags);
    }

    void GuestBuffer::Protect(const u64 vaddr, u64 size, const u32 flags) {
        size = boost::alignment::align_up(size, SwitchPageSize);
        const auto guest{VirtToGuest(vaddr, size)};
        if (guest || guest->size() < size)
            return;

        assert(descriptor.contains(guest->data()));
        assert(descriptor[guest->data()].size == size);

        assert(mprotect(guest->data(), size, flags) == 0);
    }

    void GuestBuffer::Allocate(const u64 vaddr, const u32 flags, const MemoryType type, const std::span<u8> &userdata) {
        if (!userdata.size() || type == MemoryType::Kernel)
            return;

        Map(vaddr, 0, flags, userdata.size(), type);
        Protect(vaddr, userdata.size(), MemoryProtection::Read | MemoryProtection::Write);

        const auto guest{VirtToGuest(vaddr, userdata.size())};

        const auto source{userdata.begin().base()};
        if (guest->data())
            std::memcpy(guest->data(), source, userdata.size());
        Protect(vaddr, userdata.size(), flags);
    }

    void GuestBuffer::Unmap(const u64 vaddr, u64 size) {
        const auto guest{VirtToGuest(vaddr, size)};

        if (!descriptor.contains(guest->data()))
            return;
        size = boost::alignment::align_up(size, SwitchPageSize);

        if (const auto desc = descriptor.lower_bound(guest->data()); desc != descriptor.end()) {
            if (desc->second.size == size)
                desc->second.type = MemoryType::Free;
        }

        assert(madvise(guest->data(), size, MADV_REMOVE) == 0);
    }

    std::span<u8> GuestBuffer::GetGuestSpan(const u64 vaddr) {
        const auto guest{VirtToGuest(ClearPage(vaddr), 0)};

        if (descriptor.contains(guest->data())) {
            const auto& [type, backing, bytes] = descriptor.at(guest->data());
            if (bytes)
                return std::span(guest->data(), bytes);
        }
        return {};
    }

    u64 GuestBuffer::GetUsedResourceSize() {
        u64 used{};
        u64 claimed{};
        for (const auto& blocks : std::ranges::views::values(descriptor)) {
            if (blocks.size == backing.size())
                continue;
            if (blocks.type != MemoryType::Free)
                used += blocks.size;
            else
                claimed += blocks.size;
        }
        return used + claimed;
    }
}
