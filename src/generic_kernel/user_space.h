#pragma once

#include <generic_kernel/address_space.h>
#include <generic_kernel/guest_buffer.h>
namespace Plusnx::GenericKernel {

    enum class ProgramCodeType {
        Text,
        Ro,
        Data,
    };
    struct CodeAllocationRecord {
        ProgramCodeType type;
        u64 base;
        u64 size;
        u64 used;
    };

    class UserSpace {
    public:
        UserSpace() = default;
        void CreateProcessMemory(AddressSpaceType type);
        void MapProgramCode(ProgramCodeType type, u64 baseAddr, const std::span<u8>& code);

        u8 Read8(u64 vaddr) const;
        u32 Read32(u64 vaddr) const;
        template <typename T>
        auto Read(const u64 vaddr) const {
            if constexpr (std::is_same_v<T, u32>)
                return Read32(vaddr);
            return Read8(vaddr);
        }

        void Write8(u64 vaddr, u8 value) const;
        void Write32(u64 vaddr, u32 value) const;

        AddressSpaceType type;
    private:
        void InitSelfTest() const;
        std::unique_ptr<GuestBuffer> nxmem;
        std::vector<CodeAllocationRecord> records;
    };
}
