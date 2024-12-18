#pragma once

#include <mutex>
#include <map>

#include <boost/container/small_vector.hpp>
#include <fmt/format.h>

#include <nxk/tracker.h>
namespace Plusnx::Nxk {
    class Kernel;
}
namespace Plusnx::Nxk {
    enum class MemoryType : u32 {
        Free,
        Code = 0x03FEBD04,

        Shared = 0x00402006, // Mapped using #MapSharedMemory
        Alias = 0x00482907, // Mapped using #MapMemory

        Stack = 0x005C3C0B, // Mapped during #CreateThread
        ThreadLocal = 0x0040200C, // Mapped during #CreateThread
        Kernel = 0x00002013, //Mapped in kernel during #CreateThread
    };

    class MemoryState {
    public:
        MemoryState(const MemoryType type) : value(type) {}
        union {
            struct {
                unsigned type : 7;
                unsigned protect : 1;
                unsigned debug : 1;
                unsigned useIpc : 1;
                unsigned useNonDeviceIpc : 1;
                unsigned useNonSecureIpc : 1;
                unsigned mapped : 1;
                unsigned code : 1;
                unsigned alias : 1;
                unsigned codeAlias : 1;
                unsigned transfer : 1;
                unsigned queryPhysical : 1;
                unsigned deviceMap : 1;
                unsigned alignedDeviceMap : 1;
                unsigned ipcUserBuffer : 1;
                unsigned referenceCounted : 1; // The physical memory blocks backing this region are refcounted
                unsigned mapProcess : 1;
                unsigned changeAttribute : 1;
                unsigned codeMemory : 1;
                unsigned linearMapped : 1; // [15.0.0+]
            };
            MemoryType value{};
        };

        operator auto() const {
            return value;
        }
        operator u32() const {
            return std::to_underlying(value);
        }
    };

    struct KMemoryBlockInfo {
        u8* base{};
        u64 size{};
        MemoryState state{MemoryType::Free};
        u8 permission{};
        u16 attribute{};
        MemoryState oldState{MemoryType::Free};
        u16 ipcRefCount{};
        u16 deviceMapRefCount{};
    };
    static_assert(sizeof(KMemoryBlockInfo) == 0x20);

    struct FlatMap {
        u8* base{};
        KMemoryBlockInfo* block{};
    };

    class MemoryNx {
    public:
        MemoryNx(Kernel& _kernel) : kernel(_kernel) {}
        ~MemoryNx();

        std::span<u8> AllocateSpan(u8* fixed, u64 size, bool backing) const;
        void Initialize(u64 backing, u64 board);

        u8* Allocate(u8* addr, u8* physical, u64 size, MemoryType type);
        bool Reserve(u8* addr, u8* physical, u64 size);

        bool Protect(u8* addr, u64 size, u32 protection);
        bool Free(u8* addr, u64 size);

        std::span<u8> GetUserMemory(u8* addr);
        std::pair<u8*, KMemoryBlockInfo*> SearchBlock(u8* addr);
        u64 GetUsedResourceSize();

        std::optional<std::span<u8>> guest;
        std::optional<std::span<u8>> back;
        std::optional<Tracker> tracker;
    private:
        std::map<u8*, KMemoryBlockInfo> blocks;

        boost::container::small_vector<FlatMap, 10> flatmap;

        i32 sharedFd{-1};
        std::recursive_mutex lock;
        Kernel& kernel;
    };
}

using PrintableBlock = Plusnx::Nxk::KMemoryBlockInfo;
template <>
    struct fmt::formatter<PrintableBlock> {
    template <typename ParseContext>
    static constexpr auto parse(const ParseContext& ctx) {
        return ctx.begin();
    }
    template <typename FormatContext>
    constexpr auto format(PrintableBlock const& block, FormatContext& ctx) const {
        return format_to(ctx.out(), "({}, {:X})", ptr(block.base), block.size);
    }
};
