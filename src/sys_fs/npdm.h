#pragma once

#include <sys_fs/fs_types.h>
#include <generic_kernel/address_space.h>
namespace Plusnx::SysFs {
#pragma pack(push, 1)
    enum class KeyGeneration : u32 {
    };
    struct ProcFlags {
        u32 is64BitInstruction : 1;
        GenericKernel::AddressSpaceType processAs : 3;
        u32 optimizeMemoryAllocation : 1;
        u32 disableDeviceAsMerge : 1;
        u32 enableAliasRegionExtraSize : 1;
        u32 pad0 : 1; // [19.0.0+] PreventCodeReads
    };
    struct MetaHeader {
        u32 magic; // Always "META"
        KeyGeneration signatureGen;
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
#pragma pack(pop)

    class Npdm {
    public:
        Npdm() = default;
        Npdm(const FileBackingPtr& npdm);

        void DisplayBinaryInformation() const;

        GenericKernel::AddressSpaceType environment{GenericKernel::AddressSpaceType::Guest64Bit};
    private:
        MetaHeader content{};
    };
}