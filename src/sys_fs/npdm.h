#pragma once

#include <sys_fs/fs_types.h>
#include <sys_fs/continuous_block.h>

#include <generic_kernel/address_space.h>
#include <security/key_types.h>
namespace Plusnx::SysFs {
#pragma pack(push, 1)

    struct ProcFlags {
        u32 is64BitInstruction : 1;
        GenericKernel::AddressSpaceType addressSpace : 3;
        u32 optimizeMemoryAllocation : 1;
        u32 disableDeviceAsMerge : 1;
        u32 enableAliasRegionExtraSize : 1;
        u32 pad0 : 1; // [19.0.0+] PreventCodeReads
    };
    struct MetaHeader {
        u32 magic; // Always "META"
        alignas(4) Security::KeyGeneration generation;
        u32 pad0;
        ProcFlags flags;
        u8 pad1;
        u8 mainThreadPriority;
        u8 mainThreadCoreNumber;
        u32 pad2;
        u32 systemResource;
        u32 version;
        u32 mainThreadStackSize;
        std::array<char, 0x10> name;
        std::array<char, 0x10> productCode;
        std::array<u8, 0x30> pad3;
        u32 aciOffset;
        u32 aciSize;
        u32 acidOffset;
        u32 acidSize;
    };
    static_assert(sizeof(MetaHeader) == 128);

    struct AcidHeader {
        std::array<u8, 0x200> pad0;
        u32 magic;
        u32 size;
    };
    struct Aci0Header {
        u32 magic;
    };
#pragma pack(pop)

    class Npdm {
    public:
        Npdm() = default;
        Npdm(const FileBackingPtr& npdm);

        void DisplayBinaryInformation() const;

        GenericKernel::AddressSpaceType environment{GenericKernel::AddressSpaceType::Guest64Bit};
    private:
        std::unique_ptr<ContinuousBlock> infos;

        MetaHeader content{};
        AcidHeader acid{};
        Aci0Header aci0{};
    };
}