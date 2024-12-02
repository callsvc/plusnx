#pragma once
#include <boost/container/small_vector.hpp>
#include <boost/container/flat_map.hpp>
#include <generic_kernel/constants.h>
namespace Plusnx::GenericKernel {

    enum class MemoryType : u32 {
        Free,
        Code = 0x00DC7E03, // Mapped during #CreateProcess
        Alias = 0x00482907,
        Kernel = 0x00002013,
        Stack = 0x005C3C0B,
    };

    // https://switchbrew.org/wiki/Kernel#KMemoryBlockInfo
    struct KMemoryBlockInfo {
        u8* baseAddress{nullptr};
        u64 size{};
        MemoryType state{MemoryType::Free};
        u32 permission{};
    };

    struct FlatMap {
        u64 vaddr;
        u8* base;
    };

    class GuestBuffer {
    public:
        GuestBuffer(u64 virtSize, u64 backSize);
        ~GuestBuffer();

        void Map(u64 vaddr, u64 addr, u32 flags, u64 size, MemoryType type = MemoryType::Alias);
        void Unmap(u64 vaddr, u64 size);
        void Protect(u64 vaddr, u64 size, u32 flags);

        void Allocate(u64 vaddr, u32 flags, MemoryType type, const std::span<u8>& userdata);
        u64 GetUsedResourceSize();

        std::pair<u8*, KMemoryBlockInfo> Search(u64 vaddr);
        std::span<u8> GetGuestSpan(u64 vaddr);
    private:
        std::span<u8> backing;
        std::span<u8> guest;

        // https://www.boost.org/doc/libs/1_62_0/doc/html/container/non_standard_containers.html#container.non_standard_containers.flat_xxx
        // boost::container::flat_map<u8*, KMemoryBlockInfo> blocks;
        std::map<u8*, KMemoryBlockInfo> blocks;
        boost::container::small_vector<FlatMap, 10> flatmap;
        i32 sharedFd;
    };
}