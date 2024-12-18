#include <sys_fs/fs_types.h>
#include <nxk/constants.h>
#include <nxk/user_space.h>

#include <nxk/types/kprocess.h>

namespace Plusnx::Nxk {
    void UserSpace::CreateProcessMemory(const std::shared_ptr<Types::KProcess>& process) {
        this->type = process->creation->addressType;

        u64 total{};
        u64 size{};
        if (type == AddressSpaceType::AddressSpace64Bit) {
            total = VirtualMemorySpace39::TotalSize;
            size = SwitchMainSize;

            nxmemory->Initialize(size, total);
            CreateUserAddressSpace(process, nxmemory, std::array{&code, &alias, &heap, &stack, &tlsIo});
        }

        assert(!nxmemory->GetUsedResourceSize());
#if 1
        InitSelfTest();
#endif
        nxmemory->Reserve(stack.data(), nxmemory->back->data() + 0xFF100000, 0xF00000);
    }
    u8 UserSpace::Read8(const u8* vaddr) {
        return *vaddr;
    }
    u32 UserSpace::Read32(const u8* vaddr) {
        u32 result{};
        result |= Read8(vaddr);
        result |= Read8(vaddr + 1) << 8;
        result |= Read8(vaddr + 2) << 16;
        result |= Read8(vaddr + 3) << 24;

        return result;
    }

    void UserSpace::Write8(u8* vaddr, const u8 value) {
        *vaddr = value;
    }

    void UserSpace::Write32(u8* vaddr, const u32 value) {
        Write8(vaddr, value & 0xFF);
        Write8(vaddr + 1, value >> 8 & 0xFF);
        Write8(vaddr + 2, value >> 16 & 0xFF);
        Write8(vaddr + 3, value >> 24 & 0xFF);
    }

    constexpr auto RoFlags{MemoryProtection::Read | MemoryProtection::Write};
    void UserSpace::InitSelfTest() const {
        auto* guest{nxmemory->guest->data() + 0x10000};
        auto* base{nxmemory->back->data() + 0x20000};
        auto* memory{nxmemory->Allocate(guest, base, 4, MemoryType::Shared)};

        Write32(guest, 0x41414141);
        assert(Read32(base) == 0x41414141);

        nxmemory->Free(memory, 4);
    }

    void UserSpace::MapProgramCode(const ProgramCodeType type, u8* vaddr, u8* addr, const std::span<u8>& code) {
        const auto flags = [&] {
            if (type == ProgramCodeType::Text)
                return MemoryProtection::Execute | MemoryProtection::Read;
            if (type == ProgramCodeType::Ro)
                return MemoryProtection::Read;
            return MemoryProtection::Write | MemoryProtection::Read;
        }();

        if (auto* memory{nxmemory->Allocate(vaddr, addr, code.size(), MemoryType::Code)}) {
            const auto source{code.begin().base()};
            std::memcpy(memory, source, code.size());

            nxmemory->Protect(memory, code.size(), flags);
#if 1
            if (type == ProgramCodeType::Text)
                nxmemory->tracker->ContainsCode(addr, code.size());
            else if (type == ProgramCodeType::Ro)
                assert(nxmemory->tracker->Strings(addr, code.size()).size());
#endif
        }

        if (const auto resource{nxmemory->GetUsedResourceSize()})
            records.emplace_back(type, vaddr, code.size(), resource);
    }
}
