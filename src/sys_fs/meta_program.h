#pragma once

#include <sys_fs/fs_types.h>
#include <sys_fs/streamed_file.h>

#include <nxk/address_space.h>
#include <security/key_types.h>
namespace Plusnx::SysFs {
#pragma pack(push, 1)
    struct SectionAddr {
        u32 offset;
        u32 size;
    };

    struct NpdmHeader {
        u32 magic{}; // Always "META"
        i32 keyGeneration{};
        u32 pad0{};

        struct Flags {
            u32 is64BitInstruction : 1 = {};
            Nxk::AddressSpaceType addressSpace : 3 = {};
            u32 optimizeMemoryAllocation : 1 = {};
            u32 disableDeviceAsMerge : 1 = {};
            u32 enableAliasRegionExtraSize : 1 = {};
            u32 preventCodeReads : 1 = {}; // [19.0.0+]
        } flags{};

        u8 pad1{};
        u8 mainThreadPriority{};
        u8 defaultCoreId{};
        u32 pad2{};
        u32 systemResourceSize{};
        u32 version{};
        u32 mainThreadStackSize{};
        std::array<char, 0x50> titleName{"Application"};
        SectionAddr aci0{};
        SectionAddr acid{};
    };
    static_assert(sizeof(NpdmHeader) == 0x80);

    constexpr auto FsAccessControl = 0; // FAC
    constexpr auto ServiceAccessControl = 1; // SAC
    constexpr auto KernelCapability = 2; // KC

    using ControlSections = std::array<SectionAddr, 3>;

    struct AcidHeader {
        std::array<u8, 0x200> pad0;
        u32 magic;
        u32 size;
        u8 version;
        std::array<u8, 0x3> pad1;
        u32 flags;
        u64 programIdMin;
        u64 programIdMax;
        ControlSections sec;
        u64 pad2;
    };
    static_assert(sizeof(AcidHeader) == 0x240);

    struct Aci0Header {
        u32 magic;
        std::array<u8, 0xC> pad0;
        u64 programId;
        u64 pad1;
        ControlSections sec;
        u64 pad2;
    };
    static_assert(sizeof(Aci0Header) == 0x40);
#pragma pack(pop)

    // https://github.com/SciresM/hactool/blob/master/npdm.c
    class MetaProgram {
    public:
        MetaProgram();
        MetaProgram(FileBackingPtr&& npdm);

        void DisplayBinaryInformation() const;

        void Populate(Nxk::Svc::CreateProcessParameter& creation) const;

        Nxk::AddressSpaceType addressType{};
        std::optional<std::string> title;
        std::optional<u64> titleId;

        NpdmHeader titleNpdm{};
    private:
        void SetKac(const std::vector<u32>& descriptors);

        std::unique_ptr<StreamedFile> _metaSf;


        std::map<u32, bool> allowedSvc;
        std::pair<Range<u32>, Range<u32>> priorities;

    };
}