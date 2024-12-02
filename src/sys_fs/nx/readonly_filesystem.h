#pragma once
#include <optional>

#include <sys_fs/fs_types.h>
namespace Plusnx::SysFs::Nx {
    constexpr auto RomFsEmptyEntry{0xFFFFFFFF};

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
    static_assert(sizeof(RomFsHeader) == 0x50);

    struct DirectoryEntryMeta {
        u32 parentOffset;
        u32 siblingOffset;
        u32 childOffset;
        u32 fileOffset;
        u32 hash;
        u32 nameSize;
    };
    static_assert(sizeof(DirectoryEntryMeta) == 0x18);

    struct FileEntryMeta {
        u32 parentOffset;
        u32 siblingOffset;
        u64 offset;
        u64 size;
        u32 hash;
        u32 nameSize;
    };
    static_assert(sizeof(FileEntryMeta) == 0x20);
#pragma pack(pop)

    // https://www.3dbrew.org/wiki/RomFS
    // https://github.com/switchbrew/libnx/blob/master/nx/source/runtime/devices/romfs_dev.c#L155
    class ReadOnlyFilesystem : public FileSystem {
    public:
        ReadOnlyFilesystem();
        ReadOnlyFilesystem(const FileBackingPtr& romfs);

        FileBackingPtr OpenFile(const SysPath& path, FileMode = FileMode::Read) override;
        std::vector<SysPath> ListAllFiles() const override;
    protected:
        void AddDirectory(const SysPath& path);
        void AddFile(const SysPath& path, const FileBackingPtr& file);
        void EmplaceContent(const SysPath& path, const std::string& error, BaseDirCallback&& callback);

        void VisitFiles(const FileBackingPtr& romfs, SysPath& path, u64 offset);
        void VisitSubdirectories(const FileBackingPtr& romfs, SysPath& path, u64 offset);

        RomFsHeader content;

        std::optional<Directory> filesystem;
    };
}