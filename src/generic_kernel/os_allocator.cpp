#include <sys/mman.h>
#include <boost/align/align_up.hpp>

#include <generic_kernel/constants.h>
#include <generic_kernel/os_allocator.h>
namespace Plusnx::GenericKernel {
    u8* AllocatePages(u8* start, const u64 size, const i32 descriptor, const u64 offset) {
        if (!IsAligned(size))
            return {};

        constexpr auto protection{PROT_READ | PROT_WRITE};

        constexpr auto isPrivate{MAP_ANONYMOUS | MAP_PRIVATE | MAP_NORESERVE};
        constexpr auto backing{MAP_SHARED};

        auto mapping = [&] {
            if (descriptor != -1)
                return backing;
            return isPrivate;
        }();

        if (start)
            mapping |= MAP_FIXED;

        auto* result{mmap(start, size, protection, mapping, descriptor, offset)};

        if (result == MAP_FAILED)
            throw std::bad_alloc();


        if (!start && descriptor == -1 && size > 1 * 1024 * 1024) {
            madvise(result, size, MADV_HUGEPAGE);
        } else if (!start && size >= SwitchMainSize) {
            madvise(result, size, MADV_DODUMP);
        }

        return static_cast<u8*>(result);
    }

    void FreePages(u8* pointer, const u64 size, const bool destroy) {
        if (destroy) {
            assert(munmap(pointer, size) == 0);
            return;
        }
        assert(madvise(pointer, size, MADV_REMOVE) == 0);
    }

    void ProtectPages(u8* start, const u64 size, const bool read, const bool write, const bool execute) {
        const auto flags = [&] {
            u32 protection{};
            if (read)
                protection |= PROT_READ;
            if (write)
                protection |= PROT_WRITE;
            if (execute)
                protection |= PROT_EXEC;

            return protection;
        }();
        if (!start || !size)
            return;

        assert(mprotect(start, size, flags) == 0);
    }
}
