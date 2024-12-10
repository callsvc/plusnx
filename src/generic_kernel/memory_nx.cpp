#include <mutex>
#include <ranges>

#include <fcntl.h>
#include <boost/align/align_up.hpp>
#include <boost/exception/detail/clone_current_exception.hpp>
#include <sys_fs/fsys/regular_file.h>

#include <generic_kernel/kernel.h>
#include <generic_kernel/types/kprocess.h>
#include <generic_kernel/memory_nx.h>
namespace Plusnx::GenericKernel {
    std::span<u8> MemoryNx::AllocateSpan(u8* fixed, const u64 size, const bool backing) const {
        if (!backing) {
            return std::span(AllocatePages(fixed, size), size);
        }
        return std::span(AllocatePages(fixed, size, sharedFd), size);
    }

    void MemoryNx::Initialize(const u64 backing, const u64 board) {
        assert(board > backing);

        sharedFd = shm_open("Plusnx", O_CREAT | O_RDWR, S_IRUSR | S_IWUSR);
        if (sharedFd < 0)
            return;
        assert(ftruncate64(sharedFd, backing) == 0);

        const auto position = [&] -> u8* {
            SysFs::FSys::RegularFile mapsFile{"/proc/self/maps"};
            if (!mapsFile)
                return {};

            auto content{mapsFile.GetChars(1024 * 8)};
            std::string_view line{content.data(), strlen(content.data())};
            u64 nextLine{};
            do {
                if (line.contains("[stack]")) {
                    const auto endAddress{HexTextToByteArray<8>(line.substr(line.find_first_of("-") + 1, 8 * 2))};
                    return TypeValueFromArray<u8*>(endAddress);
                }
                nextLine = line.find_first_of("\n");
                if (nextLine)
                    line = line.substr(nextLine + 1);
            } while (nextLine != std::string::npos && line.size());

            return {};
        }();

        guest.emplace(AllocateSpan(position, board, false));
        back.emplace(AllocateSpan(nullptr, backing, true));

        tracker.emplace(back->data(), back->size());

        blocks.emplace(guest->data(), KMemoryBlockInfo{.size = guest->size()});
        blocks.emplace(guest->end().base(), KMemoryBlockInfo{});
    }

    u8* MemoryNx::Allocate(u8* addr, u8* physical, const u64 size, const MemoryType type) {
        assert(addr >= guest->data() && addr < guest->end().base());

        u8* result{};
        auto firstOf{blocks.lower_bound(addr)};
        auto lastOf{blocks.lower_bound(addr + size)};
        if (const auto it{blocks.upper_bound(addr)}; it != lastOf) {
            lastOf = it;
        }

        if (tracker->Contains(physical, size))
            throw std::bad_alloc();

        if (firstOf->first > addr)
            --firstOf;
        if (lastOf->first > addr + size)
            --lastOf;

        auto allocatable{!firstOf->second.state};
        if (!allocatable)
            allocatable = firstOf->second.state == MemoryType::Alias;

        auto blockFirst{firstOf->second};
        auto blockLast{lastOf->second};

        bool reprotectIsNeeded{};

        if (firstOf == lastOf) {
            if (firstOf->first == addr && firstOf->second.state == type)
                return firstOf->first;

            blockFirst.size -= size;
            blocks.insert_or_assign(firstOf->first, blockFirst);
            blockLast.size = size;
            blocks.insert_or_assign(firstOf->first + size, blockLast);

            result = {};
        } else if (firstOf->first + size < lastOf->first && allocatable) {
            if (blockFirst.size > size) {
                blockFirst.size -= size;

                blocks.insert_or_assign(firstOf->first, blockFirst);
                blockFirst.base += size;
                blocks.insert_or_assign(firstOf->first + size, blockFirst);

                result = firstOf->first;

                reprotectIsNeeded = true;
            }
            if (blockFirst.state != type)
                reprotectIsNeeded = true;

            result = firstOf->first;
        } else if (blockFirst.size + blockLast.size > size && allocatable) {
            assert(lastOf->second.base == firstOf->first + blockFirst.size);
            firstOf->second.size += lastOf->second.size;

            if (blockFirst.state != blockLast.state)
                reprotectIsNeeded = true;
            result = firstOf->first;
            blocks.erase(lastOf);
        }

        if (!result) {
            blockFirst = KMemoryBlockInfo{
                physical, size, type, MemoryProtection::Read | MemoryProtection::Write
            };
            assert(tracker->Allocate(physical, size) != nullptr);

            const auto offset{static_cast<u64>(physical - back->data())};
            result = AllocatePages(addr, size, sharedFd, offset);

            if (auto limits{kernel.GetCurrentProcess()->npdm.titleNpdm.systemResourceSize}) {
                if (blocks.size() > limits)
                    throw std::bad_alloc();
            }

            blocks.insert_or_assign(addr, blockFirst);
        }
        if (reprotectIsNeeded) {
            assert(MemoryState{type}.protect == 0);

            auto [base, block] = SearchBlock(addr);
            assert(base == result);

            if (block->oldState == type) {
                block->state = type;
            }
            block->state = type;
        }

        auto flat{flatmap.begin()};

        const auto resetCachedMap = [&] {
            for (; flat != flatmap.end(); ++flat) {
                if (addr >= flat->base && flat->base + size < addr)
                    return true;
            }
            return false;
        }();

        if (resetCachedMap) {
            flatmap.clear();
        }

        return result;
    }

    bool MemoryNx::Reserve(u8* addr, u8* physical, const u64 size) {
        std::scoped_lock guard(lock);

        const auto [base, _] = SearchBlock(addr);
        if (base || tracker->Contains(physical, size))
            return {};

        const auto* result{Allocate(addr, physical, size, MemoryType::Alias)};
        return result && SearchBlock(addr).first != nullptr;
    }

    bool MemoryNx::Protect(u8* addr, u64 size, const u32 protection) {

        const auto [_, block] = SearchBlock(addr);
        if (!block)
            return {};

        size = boost::alignment::align_up(size, SwitchPageSize);
        if (block->size != size)
            return {};

        if (block->permission == protection || !block->state.protect)
            return {};
        ProtectPages(addr, size, protection & MemoryProtection::Read, protection & MemoryProtection::Write, protection & MemoryProtection::Execute);
        block->permission = protection;

        return true;
    }

    bool MemoryNx::Free(u8* addr, u64 size) {
        const auto [base, block] = SearchBlock(addr);
        if (!base || !block)
            return {};

        size = boost::alignment::align_up(size, SwitchPageSize);
        assert(IsAligned(size));

        if (size != block->size) {
            // Split the block into two, preserving the upper block if the size does not fill the reserved space
            auto copyBlock{*block};
            copyBlock.size = block->size - size;
            copyBlock.base += size;

            block->size -= copyBlock.size;

            blocks.insert_or_assign(addr + size, copyBlock);
        }
        block->oldState = block->state;
        block->state = MemoryType::Free;

        FreePages(base, size);
        tracker->Free(block->base);

        return true;
    }

    MemoryNx::~MemoryNx() {
        if (sharedFd > 0)
            shm_unlink("Plusnx");

        if (guest)
            FreePages(guest->data(), back->size(), true);
        if (back)
            FreePages(back->data(), back->size(), true);

        flatmap.clear();
        blocks.clear();
        close(sharedFd);
    }

    std::span<u8> MemoryNx::GetUserMemory(u8* addr) {
        std::scoped_lock guard(lock);
        auto [base, block] = SearchBlock(addr);

        if (blocks.contains(base)) {
            assert(block->state != MemoryType::Free);
            if (block->size)
                return std::span(addr, block->size);
        }
        return {};
    }

    std::pair<u8*, KMemoryBlockInfo*> MemoryNx::SearchBlock(u8* addr) {
        for (const auto& [_vaddr, block] : flatmap) {
            if (_vaddr != addr)
                continue;
            assert(blocks.contains(_vaddr));
            return std::make_pair(_vaddr, block);
        }

        auto base{blocks.lower_bound(addr)};
        if (base == blocks.end())
            return {};

        if (base->first > addr && base != blocks.begin())
            --base;
        if (addr >= base->first && base->first + base->second.size < addr)
            return {};
        if (base->first) {
            flatmap.emplace_back(base->first, &base->second);
            return std::make_pair(base->first, &base->second);
        }
        return {};
    }

    u64 MemoryNx::GetUsedResourceSize() {
        std::scoped_lock guard(lock);

        const auto totalMemory = [&] -> u64 {
            u64 used{};
            for (const auto& block : std::ranges::views::values(blocks)) {
                if (block.state != MemoryType::Free)
                    used += block.size;
            }
            return used;
        }();
        const auto resourceUsage = [&] -> u64 {
            if (const auto process{kernel.GetCurrentProcess()})
                if (const auto& properties{process->npdm}; *properties.titleId)
                    return std::min(properties.titleNpdm.systemResourceSize, static_cast<u32>(blocks.size()));

            return {};
        }();

        if (resourceUsage)
            return std::min(totalMemory, resourceUsage);
        return totalMemory;
    }
}
