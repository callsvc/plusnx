#include <generic_kernel/constants.h>
#include <sys_fs/fs_types.h>
#include <generic_kernel/user_space.h>
namespace Plusnx::GenericKernel {
    void UserSpace::CreateProcessMemory(const AddressSpaceType type) {
        this->type = type;

        u64 total{};
        u64 size{};
        if (type == AddressSpaceType::AddressSpace64Bit) {
            total = VirtualMemorySpace39::TotalSize;
            size = SwitchMainSize;
        }
        nxmem = std::make_unique<GuestBuffer>(total, size);
#if 1
        InitSelfTest();
#endif
    }
    void UserSpace::MapProgramCode(const ProgramCodeType type, const u64 baseAddr, const std::span<u8>& code) {
        const auto flags = [&] {
            if (type == ProgramCodeType::Text)
                return MemoryProtection::Execute | MemoryProtection::Read;
            if (type == ProgramCodeType::Ro)
                return MemoryProtection::Read;
            return MemoryProtection::Write | MemoryProtection::Read;
        }();

        nxmem->Allocate(baseAddr, flags, MemoryType::Code, code);

        records.emplace_back(type, baseAddr, code.size(), nxmem->GetUsedResourceSize());
        std::print("Amount of allocated data: {}\n", SysFs::GetReadableSize(records.back().used));
    }

    u8 UserSpace::Read8(const u64 vaddr) const {
        const auto memory{nxmem->GetGuestSpan(vaddr)};
        return memory[PageIndex(vaddr)];
    }

    u32 UserSpace::Read32(const u64 vaddr) const {
        u32 result{};
        result |= Read8(vaddr);
        result |= Read8(vaddr + 1) << 8;
        result |= Read8(vaddr + 2) << 16;
        result |= Read8(vaddr + 3) << 24;

        return result;
    }

    void UserSpace::Write8(const u64 vaddr, const u8 value) const {
        const auto memory{nxmem->GetGuestSpan(vaddr)};
        memory[PageIndex(vaddr)] = value;
    }

    void UserSpace::Write32(const u64 vaddr, const u32 value) const {
        Write8(vaddr, value & 0xFF);
        Write8(vaddr + 1, value >> 8 & 0xFF);
        Write8(vaddr + 2, value >> 16 & 0xFF);
        Write8(vaddr + 3, value >> 24 & 0xFF);
    }


    constexpr auto RoFlags{MemoryProtection::Read | MemoryProtection::Write};
    void UserSpace::InitSelfTest() const {
        nxmem->Map(0x10000, 0x20000, RoFlags, sizeof(u32));

        Write32(0x10000, 0x41414141);
        assert(Read32(0x10000) == 0x41414141);

        nxmem->Unmap(0x10000, sizeof(u32));
    }
}
