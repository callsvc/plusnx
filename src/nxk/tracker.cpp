#include <fstream>
#include <armored/frontend/mach_arm64_disas.h>
#include <sys_fs/fsys/regular_file.h>

#include <nxk/tracker.h>
namespace Plusnx::Nxk {
    Tracker::Tracker(u8* base, u64 size) {
        mapper.emplace(base, size);

        u64 countMax{65530};
        // https://docs.kernel.org/admin-guide/sysctl/vm.html#max-map-count
        if (SysFs::FSys::RegularFile maxMapCount{"/proc/sys/vm/max_map_count"}) {
            const auto content{maxMapCount.GetChars(sizeof(u64) * 8)};
            countMax = std::strtoull(content.data(), nullptr, 10);
        }
        mapCountBarrier = countMax;
    }

    u8* Tracker::Allocate(u8* pointer, const u64 size) {
        if (const auto block{mapper.find(pointer)}; block != mapper.end()) {
            fmt::println("A larger block has already been allocated, {} total size {}", fmt::ptr(block->first), block->second.size);
            return {};
        }
        if (residentMaps == mapCountBarrier)
            throw std::bad_alloc();

        if (const auto leftBlock{mapper.begin()}; leftBlock != mapper.end()) {
            leftBlock->second.size -= size;
        }
        mapper.emplace(pointer, AllocationRecord{size, true});
        commited += size;
        residentMaps++;
        return pointer + size;
    }

    void Tracker::Free(u8* pointer) {
        if (mapper.contains(pointer)) {
            if (!mapper[pointer].allocated)
                return;
        } else {
            return;
        }

        const auto [size, _] = mapper[pointer];
        if (const auto first{mapper.begin()}; first != mapper.end() && size) {
            first->second.size += size;
        }
        commited -= size;
        [[assume(residentMaps > 0)]];
        residentMaps--;
        mapper.erase(pointer);
    }

    i32 Tracker::Contains(u8* pointer, const u64 size) const {
        if (!mapper.size())
            return {};

        auto block{mapper.lower_bound(pointer)};
        while (block->first > pointer && block != mapper.begin())
            --block;
        if (!block->second.allocated)
            return {};

        if (block->first != pointer)
            if (block->first + block->second.size > pointer)
                return 1;

        return block->second.size == size ? 2 : 1;
    }

    std::vector<std::string> Tracker::Strings(u8* pointer, u64 size) const {
        std::vector<std::string> result;
        result.reserve(30);

        if (!Contains(pointer, size)) {
            return {};
        }

        const auto* src{reinterpret_cast<const char*>(pointer)};
        constexpr auto MinimumStringLength{6};
        constexpr auto MaxStringsCount{128};
        for (u64 rate{}; size-- && result.size() < MaxStringsCount; rate++) {
            if (!isalpha(*src++))
                rate = {};
            if (rate < MinimumStringLength)
                continue;

            src -= MinimumStringLength;
            std::string strings{src};
            auto invalid{std::ranges::find_if(strings, [](const char _char) { return !isascii(_char); })};
            if (invalid != strings.end())
                strings.erase(invalid, strings.end());

            src += strings.size();

            result.emplace_back(std::move(strings));
            rate = {};
        }
#if 1
    if (std::fstream strings{"user/strings.txt", std::ios::out | std::ios::app}) {
        strings << std::format("Some strings captured from the data region:\n\n");
        for (const auto& string : result) {
            strings << std::format("{}\n", string);
        }
        strings.flush();
    }
#endif
        return result;
    }

    bool Tracker::ContainsCode(u8* pointer, const u64 size) const {
        if (!Contains(pointer, size)) {
            return {};
        }
        constexpr auto MinimalCoherence{8};
        for (u64 minimum{}, index{}; index < size && minimum != MinimalCoherence; index += 4, minimum++) {
            u32 instruction{};
            std::memcpy(&instruction, pointer, 4);

            pointer += 4;
            if (!Armored::Frontend::IsArm64Code(instruction))
                return {};
        }
        return true;
    }
}
