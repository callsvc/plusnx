#pragma once

#include <types.h>
#include <sys/mman.h>
namespace Plusnx::GenericKernel {
    constexpr u64 SwitchPageSize{4096};

    inline auto ClearPage(const u64 address) {
        return address & ~(SwitchPageSize - 1);
    }
    inline auto PageIndex(const u64 address) {
        constexpr auto clear{SwitchPageSize - 1};
        return address & clear;
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