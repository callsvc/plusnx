#pragma once

#include <fmt/format.h>

#include <nxk/os_allocator.h>
namespace Plusnx::Nxk {
    struct AllocationRecord {
        u64 size;
        bool allocated{false};
    };
    class Tracker {
    public:
        Tracker(u8* base, u64 size);

        u8* Allocate(u8* pointer, u64 size);
        void Free(u8* pointer);

        i32 Contains(u8* pointer, u64 size) const;
        bool ContainsCode(u8* pointer, u64 size) const;
        std::vector<std::string> Strings(u8* pointer, u64 size) const;

    private:
        std::map<u8*, AllocationRecord> mapper;
        u64 commited{};
        u64 residentMaps{};
        u64 mapCountBarrier{};
    };
}