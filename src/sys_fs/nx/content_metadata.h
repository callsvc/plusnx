#pragma once
#include <sys_fs/fs_types.h>

namespace Plusnx::SysFs::Nx {
    enum class ContainedFormat {
        Invalid,
        Nca,
        Ticket,
        Cnmt,
        Xml
    };
    ContainedFormat GetEntryFormat(const SysPath& pathname);

    enum class ContentMetaType : u8 {
        Invalid,
        SystemProgram,
        SystemData,
        SystemUpdate,
        BootImagePackage,
        BootImagePackageSafe,
        Application = 0x80,
        Patch,
        AddonContent,
        Delta,
        DataPatch // [15.0.0+]
    };
#pragma pack(push, 1)
    struct CnmtHeader {
        u64 id;
        u32 version;
        ContentMetaType type;
    };
#pragma pack(pop)

    class ContentMetadata {
    public:
        explicit ContentMetadata(const FileBackingPtr& cnmt);
        u64 programId;
        ContentMetaType type;
    private:
        CnmtHeader content;
    };
}
