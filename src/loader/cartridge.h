#pragma once

#include <loader/app_loader.h>

#include <sys_fs/nx/nca_core.h>
namespace Plusnx::Loader {
    enum class RomSize : u8 {
        MinorCard0 = 0xFA,
        MinorCard1 = 0xF8,
        MediumCard0 = 0xF0,
        MediumCard1 = 0xE0,
        BiggerCard0 = 0xE1,
        BiggerCard1 = 0xE2
    };

#pragma pack(push, 1)
    struct XciFirstHeader {
        std::array<u8, 0x100> signature;
        u32 magic; // Magic ("HEAD")
        u32 romStartAddr;
        u32 backupArea; // always (0xFFFFFFFF)
        u8 highTitleKeyIndex;
        RomSize size;
        u8 version;
        u8 flags;
        u64 packageId;
        u32 validDataEndAddr;
        u8 pad0;
        u8 flags2; // [18.0.0+]
        u16 pad1;
        std::array<u8, 0x10> iv;
        u64 pfsBeginAddr;
        u64 pfsSize;
        std::array<u8, 0x20> pfsHeaderHash;
        std::array<u8, 0xA0> pad3;
    };
    static_assert(sizeof(XciFirstHeader) == 0x190 + 0x70);

    struct XciHeader {
        // std::array<u8, 0x1000> cardKeyArea;
        XciFirstHeader cardHeader;
    };

#pragma pack(pop)

    // https://switchbrew.org/wiki/XCI
    class Cartridge final : public AppLoader {
    public:
        Cartridge(const std::shared_ptr<Security::Keyring>& _keys, const SysFs::FileBackingPtr& xci);
        void Load(std::shared_ptr<Core::Context>& context) override;

        std::unique_ptr<SysFs::Nx::PartitionFilesystem> pfs;
        SysFs::FileBackingPtr GetNpdm() const override;
    private:
        void ListAllNca(const std::unique_ptr<SysFs::Nx::PartitionFilesystem>& partition);

        std::shared_ptr<Security::Keyring> keys;
        std::shared_ptr<SysFs::Nx::PartitionFilesystem> exefs;
        std::shared_ptr<SysFs::Nx::PartitionFilesystem> logo;

        std::shared_ptr<SysFs::Nx::ReadOnlyFilesystem> romfs;
        std::shared_ptr<SysFs::Nx::ReadOnlyFilesystem> control;

        XciHeader content;
    };
}
