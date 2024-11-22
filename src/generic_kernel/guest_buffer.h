#pragma once
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
    struct VAddrPool {
        MemoryType type;
        u8* base;
        u64 size;
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

        std::span<u8> GetGuestSpan(u64 vaddr);
    private:
        std::optional<std::span<u8>> VirtToGuest(u64 vaddr, u64 size);

        std::span<u8> backing;
        std::span<u8> guest;

        // https://www.boost.org/doc/libs/1_62_0/doc/html/container/non_standard_containers.html#container.non_standard_containers.flat_xxx
        boost::container::flat_map<u8*, VAddrPool> descriptor;
        i32 mfd;
    };
}