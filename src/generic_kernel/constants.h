#pragma once

#include <types.h>
#include <sys/mman.h>
namespace Plusnx::GenericKernel {
    constexpr u64 SwitchPageSize{4096};

    inline auto ClearSwitchPage(const u64 address) {
        return address & ~(SwitchPageSize - 1);
    }
    inline auto IndexPage(const u64 address, const u64 size) {
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

    namespace VirtualMemorySpace39 {
        constexpr u64 TotalSize{1ULL << 39};
    }
}