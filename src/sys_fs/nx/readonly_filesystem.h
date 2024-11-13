#pragma once
#include <optional>

#include <sys_fs/fs_types.h>
namespace Plusnx::SysFs::Nx {
    constexpr auto RomFsEmptyEntry{0xffffffff};

#pragma pack(push, 1)
    struct RomFsHeader {
        u64 size;
        u64 dirHashOffset;
        u64 dirHashSize;
        u64 dirMetaOffset;
        u64 dirMetaSize;
        u64 fileHashOffset;
        u64 fileHashSize;
        u64 fileMetaOffset;
        u64 fileMetaSize;

        u64 fileDataOffset;
    };

    struct DirectoryEntryMeta {
        u32 parentOffset;
        u32 nextDirSiblingOffset;
        u32 childDirOffset;
        u32 firstFileOffset;
        u32 nextDirOffset;
        u32 nameLength;
    };
    static_assert(sizeof(DirectoryEntryMeta) == 0x18);

    struct FileEntryMeta {
        u32 parentOffset;
        u32 nextFileSiblingOffset;
        u64 dataOffset;
        u64 size;
        u32 nextFileOffset;
        u32 nameLength;
    };
    static_assert(sizeof(FileEntryMeta) == 0x20);
#pragma pack(pop)

    // https://www.3dbrew.org/wiki/RomFS
    class ReadOnlyFilesystem final : public FileSystem {
    public:
        ReadOnlyFilesystem(const FileBackingPtr& romfs);

        FileBackingPtr OpenFile(const SysPath& path, FileMode = FileMode::Read) override;
        std::vector<SysPath> ListAllFiles() const override;
    private:
        void AddDirectory(const SysPath& path);
        void AddFile(const SysPath& path, const FileBackingPtr& file);
        void EmplaceContent(const SysPath& path, const std::string& error, BaseDirCallback&& callback);

        void VisitFiles(const FileBackingPtr& romfs, SysPath& path, u64 offset);
        void VisitSubdirectories(const FileBackingPtr& romfs, SysPath& path, u64 offset);

        RomFsHeader content;

        std::optional<std::pair<SysPath, Directory>> filesystem;
    };
}