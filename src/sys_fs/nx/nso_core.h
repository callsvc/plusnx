#pragma once

#include <sys_fs/fs_types.h>
namespace Plusnx::SysFs::Nx {
#pragma pack(push, 1)
    struct NsoSegment {
        u32 fileOffset;
        u32 offset;
        u32 size;
    };
    struct NsoRegion {
        u32 offset;
        u32 size;
    };
    struct NsoHeader {
        u32 magic; // Signature ("NSO0")
        u32 version;
        u32 pad0;
        u32 flags;
        NsoSegment text;
        u32 moduleNameOffset;
        NsoSegment ro;
        u32 moduleNameSize;
        NsoSegment data;
        u32 bssSize;
        std::array<u8, 0x20> moduleId;
        u32 textFileSize;
        u32 roFileSize;
        u32 dataFileSize;
        std::array<u8, 0x1C> pad1;
        NsoRegion embedded;
        NsoRegion dynStr; // relative to the .rodata section)
        NsoRegion dynSym; // relative to the .rodata section)
        std::array<std::array<u8, 0x20>, 3> hashList;
    };
#pragma pack(pop)

    class NsoCore {
    public:
        NsoCore(const FileBackingPtr& nso);
    private:
        NsoHeader content;
    };
}