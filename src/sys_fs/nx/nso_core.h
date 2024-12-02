#pragma once

#include <sys_fs/fs_types.h>
#include <loader/app_loader.h>
namespace Plusnx::SysFs::Nx {
#pragma pack(push, 1)
    struct NsoSection {
        u32 fileOffset;
        u32 offset;
        u32 size;
    };
    struct NsoSegment {
        u32 offset;
        union {
            u32 size;
            u32 uncompressed;
        };
    };
    struct NsoHeader {
        u32 magic; // Signature ("NSO0")
        u32 version;
        u32 pad0;
        u32 flags;
        NsoSection text;
        u32 moduleNameOffset;
        NsoSection ro;
        u32 moduleNameSize;
        NsoSection data;
        u32 bssSize;
        std::array<u8, 0x20> moduleId;
        u32 textFileSize;
        u32 roFileSize;
        u32 dataFileSize;
        std::array<u8, 0x1C> pad1;
        NsoSegment embedded;
        NsoSegment dynStr; // relative to the .rodata section)
        NsoSegment dynSym; // relative to the .rodata section)
        std::array<std::array<u8, 0x20>, 3> hashList;
    };
#pragma pack(pop)

    class NsoCore {
    public:
        NsoCore(const FileBackingPtr& nso);
        void Load(const std::shared_ptr<GenericKernel::Types::KProcess>& process, u64& address, bool hasArguments = true);

        std::optional<std::string> moduleName;
    private:
        bool IsSectionCompressed(Loader::SectionType type) const;
        bool HashableSection(Loader::SectionType type) const;
        void ReadSection(const NsoSection& section, u64 fileSize, std::span<u8>& output);
        Loader::SectionType GetSectionType(const NsoSection& section) const;

        std::map<Loader::SectionType, std::array<u8, 0x20>> sectionResults;

        FileBackingPtr backing;

        NsoHeader content;
        std::vector<u8> program;

        std::span<u8> textSection;
        std::span<u8> roSection;
        std::span<u8> dataSection;
    };
}