#pragma once

#include <loader/app_loader.h>
namespace Plusnx::Loader {
#pragma pack(push, 1)
    struct SegmentAddr {
        u32 offset;
        u32 size;
    };

    struct NroHeader {
        std::array<u8, 0x10> pad0;
        u32 magic; // Signature ("NRO0")
        u32 version;
        u32 size;
        u32 flags;
        SegmentAddr text;
        SegmentAddr ro;
        SegmentAddr data;
        SegmentAddr bss;
        std::array<u8, 0x20> pad1;
        u32 dsoHandleOffset;
        u32 pad2;
        SegmentAddr embedded;
        SegmentAddr dynStr;
        SegmentAddr dynSym;
    };

    struct AssetSection {
        u64 offset;
        u64 size; // 0 indicates this section is empty
    };

    enum class AssetType : u64 {
        Icon,
        Nacp,
        RomFs
    };
    struct AssetHeader {
        u32 magic;
        u32 version;

        std::array<AssetSection, 3> assets;
    };
    static_assert(sizeof(NroHeader) == 0x80);
    static_assert(sizeof(AssetHeader) == 0x38);
#pragma pack(pop)

    // https://switchbrew.org/wiki/NRO
    class NxExecutable final : public AppLoader {
    public:
        NxExecutable(const SysFs::FileBackingPtr& nro);
        void ReadAssets(const SysFs::FileBackingPtr& nro);

        std::span<u8> ReadSectionContent(const SysFs::FileBackingPtr& nro, SectionType type, u32 fileOffset);
        void Load(std::shared_ptr<Core::Context>& context) override;
        std::shared_ptr<SysFs::Nx::ReadOnlyFilesystem> GetRomFs(bool isControl) const override;

        std::shared_ptr<SysFs::Nx::ReadOnlyFilesystem> romfs;
        std::shared_ptr<SysFs::Nx::PartitionFilesystem> icon;

        std::shared_ptr<SysFs::FileBacking> nacp;

        std::span<u8> textSection;
        std::span<u8> roSection;
        std::span<u8> dataSection;

        std::span<u8> GetExeSection(SectionType type) const override;
        SysFs::FileBackingPtr GetNpdm() const override {
            // NRO is the executable format for non-ExeFS binaries
            return {};
        }

    private:
        NroHeader content;
        AssetHeader assetHeader;

        std::vector<u8> program;
    };
}
