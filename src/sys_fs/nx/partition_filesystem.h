#pragma once

#include <vector>
#include <unordered_map>

#include <sys_fs/fs_types.h>
#include <types.h>
namespace Plusnx::SysFs::Nx {
#pragma pack(push, 1)
    struct FileEntry {
        u64 offset;
        u64 size;
        u32 nameOffset;
    };
    struct ContentEntry : FileEntry {
        u32 pad0;
    };
    struct HashableContentEntry : FileEntry {
        u32 regionSize;
        u64 zeroed;
        std::array<u8, 0x20> hash;
    };
#pragma pack(pop)

    class PartitionFilesystem final : public RoDirectoryBacking {
    public:
        PartitionFilesystem(const FileBackingPtr& pfs);
        std::vector<SysPath> ListAllFiles() const override;
        FileBackingPtr OpenFile(const SysPath& path, FileMode = FileMode::Read) override;

        struct SuperBlock {
            u32 magic;
            u32 entries;
            u32 strTableSize;
            u32 pad0;
        };

    private:
        std::unordered_map<SysPath, FileEntry> entries;
        bool hashable{};
        u64 entrySize;
        u64 dataOffset;
        SuperBlock block;

        std::vector<u8> content;
        const FileBackingPtr backing;
    };

    class StringTable {
    public:
        explicit StringTable(u32 offset, u32 size, const FileBackingPtr& pfs);
        std::string ReadString(u32 offset) const;

        u64 offset;
        u64 size;
    private:
        std::vector<char> table;
    };

    bool IsAExeFsPartition(const std::unique_ptr<PartitionFilesystem>& pfs);
}
