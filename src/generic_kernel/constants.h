#pragma once

#include <types.h>
#include <sys/mman.h>
namespace Plusnx::GenericKernel {
    constexpr u64 SwitchPageSize{4 * 1024};
    constexpr u64 UserSlabHeapItemSize{SwitchPageSize};
    constexpr u64 UserSlabHeapSize{0x3DE000};
    constexpr u64 UserSlabBase{0x80000000 + 0x60000 + 0x85000};

    inline auto ClearSwitchPage(const u64 address) {
        return address & ~(SwitchPageSize - 1);
    }
    inline auto IndexPage(const u64 address, const u64 size = SwitchPageSize) {
        auto pageBitsCount{std::countl_zero(size)};
        pageBitsCount = sizeof(std::uint64_t) * 8 - pageBitsCount - 1;
        // ReSharper disable once CppRedundantParentheses
        return address & ((1 << pageBitsCount) - 1);
    }

    namespace MemoryProtection {
#if defined(__linux__)
        constexpr auto Read{PROT_READ};
        constexpr auto Write{PROT_WRITE};
        constexpr auto Execute{PROT_EXEC}; // NCE mode only
#endif
    }

    constexpr u64 SwitchMainSize{4ULL * 1024 * 1024 * 1024};
    constexpr u64 RegionAlignment{0x200000};
    namespace VirtualMemorySpace39 {
        // https://github.com/strato-emu/strato/blob/master/app/src/main/cpp/skyline/kernel/memory.cpp#L208
        constexpr u64 TotalSize{1ULL << 39};
        constexpr u64 AliasSize{0x1000000000};
        constexpr u64 HeapSize{0x180000000};
        constexpr u64 StackSize{0x80000000};
        constexpr u64 TlsIoSize{0x1000000000};
    }

    using RegionProperties = std::span<u8>;
}