#pragma once

#include <generic_kernel/address_space.h>
#include <generic_kernel/svc/svc_types.h>
namespace Plusnx::GenericKernel {

    enum class ProgramCodeType {
        Text,
        Ro,
        Data,
    };
    struct CodeAllocationRecord {
        ProgramCodeType type;
        u8* base;
        u64 size;
        u64 used;
    };

    namespace Types {
        class KProcess;
    }

    class UserSpace {
    public:
        UserSpace(std::unique_ptr<GuestBuffer>& guest) : nxmemory(guest) {
        }
        void CreateProcessMemory(const std::shared_ptr<Types::KProcess>& process);
        void MapProgramCode(ProgramCodeType type, u8* vaddr, u8* addr, const std::span<u8>& code);

        static u8 Read8(const u8* vaddr);

        static u32 Read32(const u8* vaddr);
        template <typename T>
        auto Read(const u8* vaddr) const {
            if constexpr (std::is_same_v<T, u32>)
                return Read32(vaddr);
            return Read8(vaddr);
        }

        static void Write8(u8* vaddr, u8 value);

        static void Write32(u8* vaddr, u32 value);

        AddressSpaceType type{};
        std::vector<CodeAllocationRecord> records;

        RegionProperties code{};
        RegionProperties alias{};
        RegionProperties heap{};
        RegionProperties stack{};
        RegionProperties tlsIo{};
    private:
        void InitSelfTest() const;
        std::unique_ptr<GuestBuffer>& nxmemory;
    };
}
