#pragma once
#include <boost/container/small_vector.hpp>
#include <boost/container/flat_map.hpp>
#include <generic_kernel/constants.h>
namespace Plusnx::GenericKernel {

    enum class MemoryType : u32 {
        Free,
        Code = 0x00DC7E03, // Mapped during #CreateProcess
        Shared = 0x00402006,
        Alias = 0x00482907,
        Stack = 0x005C3C0B,
        ThreadLocal = 0x0040200C, // Mapped during #CreateThread
        Kernel = 0x00002013,
    };

    // https://switchbrew.org/wiki/Kernel#KMemoryBlockInfo
    struct KMemoryBlockInfo {
        u8* base{nullptr};
        u64 size{};
        MemoryType state{MemoryType::Free};
        u32 permission{};
    };

    struct FlatMap {
        u8* vaddr;
        KMemoryBlockInfo* block;
    };

    class GuestBuffer {
    public:
        GuestBuffer() = default;
        ~GuestBuffer();

        void Initialize(u64 virtSize, u64 backSize);

        u8* Allocate(u8* vaddr, u8* addr, u32 flags, u64 size, MemoryType type = MemoryType::Alias);
        void Free(u8* address, u64 size);
        void Protect(u8* vaddr, u64 size, u32 flags);

        u64 GetUsedResourceSize();

        std::pair<u8*, KMemoryBlockInfo*> Search(u8* vaddr);

        std::span<u8> GetGuestSpan(u8* vaddr);
        std::optional<std::span<u8>> guest;
        std::optional<std::span<u8>> backing;

    private:
        // https://www.boost.org/doc/libs/1_62_0/doc/html/container/non_standard_containers.html#container.non_standard_containers.flat_xxx
        boost::container::flat_map<u8*, KMemoryBlockInfo> blocks;
        boost::container::small_vector<FlatMap, 10> flatmap;
        i32 sharedFd;

        std::mutex lock;
    };
}