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
        void Load(std::shared_ptr<Core::Context> &process) override;
    private:
        NroHeader content;
        AssetHeader assetHeader;

        std::vector<u8> program;
    };
}
