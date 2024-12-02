#pragma once

#include <boost/align/align_up.hpp>
#include <types.h>

#include <sys/mman.h>
namespace Plusnx::Details {
    template <typename T>
    class VirtualAllocator {
    public:
        using value_type = T;
        VirtualAllocator() noexcept = default;

        static auto GetAlignedSize(const u64 count) {
            return boost::alignment::align_up(count * sizeof(T), 4096);
        }

        static T* allocate(const std::size_t count) {
            auto result{mmap(nullptr, GetAlignedSize(count), PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0)};
            if (result == MAP_FAILED) {
                throw std::bad_alloc();
            }
            return static_cast<T*>(result);
        }

        static void deallocate(T* ptr, const u64 count) {
            if (!ptr)
                return;
            munmap(ptr, GetAlignedSize(count));
        }
    };
}