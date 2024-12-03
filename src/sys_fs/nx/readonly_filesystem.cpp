#include <boost/align/align_up.hpp>
#include <ranges>

#include <sys_fs/layered_fs.h>
#include <sys_fs/nx/readonly_filesystem.h>

namespace Plusnx::SysFs::Nx {
    ReadOnlyFilesystem::ReadOnlyFilesystem() : FileSystem("Zeroed"), content() {}
    ReadOnlyFilesystem::ReadOnlyFilesystem(const FileBackingPtr& romfs) : FileSystem(romfs->path) {
        if (romfs->Read(content) != sizeof(RomFsHeader))
            return;
        assert(sizeof(RomFsHeader) == content.size);

        SysPath root{"/"};
        if (!filesystem)
            AddDirectory({});
        VisitSubdirectories(romfs, root, 0);

        const auto filesTable(romfs->GetBytes<u32>(content.fileHashSize / sizeof(u32), content.fileHashOffset));
        const auto countOfFiles = [&] {
            u32 count{};
            for (const auto& file : filesTable)
                if (file != RomFsEmptyEntry)
                    count++;
            return count;
        }();

        const auto files{ReadOnlyFilesystem::ListAllFiles()};
        if (countOfFiles < files.size()) {
            std::print("File count exceeds the available hashes in this RomFS\n");
        }
        u64 totalSize{};
        for (const auto& file : files) {
            totalSize += ReadOnlyFilesystem::OpenFile(file)->GetSize();
        }
        std::print("File count {}, total file bundle size {}\n", files.size(), GetReadableSize(totalSize));
    }

    void AppendEntryName(const FileBackingPtr& romfs, SysPath& path, const u64 length, const u64 offset) {
        const auto dirName{romfs->GetChars(boost::alignment::align_up(length, 4), offset)};
        path.append(std::string_view(dirName.data(), length));
    }

    void ReadOnlyFilesystem::VisitSubdirectories(const FileBackingPtr& romfs, SysPath& path, u64 offset) {
        u32 dirOffset{};
        auto PopulateSubdirectory = [&] (const DirectoryEntryMeta& directory) {
            if (directory.nameSize) {
                AppendEntryName(romfs, path, directory.nameSize, dirOffset + sizeof(directory));
                AddDirectory(path);
            }
            if (directory.fileOffset != RomFsEmptyEntry)
                VisitFiles(romfs, path, directory.fileOffset);

            if (directory.childOffset != RomFsEmptyEntry) {
                VisitSubdirectories(romfs, path, directory.childOffset);
            }
        };
        DirectoryEntryMeta directory{};
        do {
            dirOffset = content.dirMetaOffset + offset;
            romfs->Read(directory, dirOffset);

            PopulateSubdirectory(directory);
            if (path.has_parent_path())
                path = path.parent_path();

            offset = directory.siblingOffset;
        } while (directory.siblingOffset != RomFsEmptyEntry);
    }

    void ReadOnlyFilesystem::VisitFiles(const FileBackingPtr& romfs, SysPath& path, u64 offset) {
        FileEntryMeta file{};
        u32 fileOffset{};
        do {
            fileOffset = content.fileMetaOffset + offset;
            romfs->Read(file, fileOffset);
            if (file.nameSize == RomFsEmptyEntry)
                return;

            if (file.nameSize) {
                AppendEntryName(romfs, path, file.nameSize, fileOffset + sizeof(file));
                AddFile(path, std::make_shared<FileLayered>(romfs, path, content.fileDataOffset + file.offset, file.size));
                path = path.parent_path();
            }

            offset = file.siblingOffset;
        } while (file.siblingOffset != RomFsEmptyEntry);
    }

    FileBackingPtr ReadOnlyFilesystem::OpenFile(const SysPath& path, const FileMode mode) {
        assert(mode == FileMode::Read);
        FileBackingPtr result;
        auto OpenFileWithin = [&](const Directory& directory, const auto& parent) {
            for (const auto& file : std::ranges::views::values(directory.files)) {
                if (file->path.parent_path() == parent)
                    if (file->path == path)
                        result = file;
            }
            return result != nullptr;
        };

        WalkDirectories(*filesystem, path.parent_path(), OpenFileWithin);
        return result;
    }
    std::vector<SysPath> ReadOnlyFilesystem::ListAllFiles() const {
        std::vector<SysPath> files;
        std::function<void(std::vector<SysPath>&, const Directory&)> ListAllTopLevelDirs = [&](auto& result, const auto& directory) {
            for (const auto& file : directory.files | std::views::values) {
                result.emplace_back(file->path);
            }
            for (const auto& subdir : directory.subdirs | std::views::values) {
                ListAllTopLevelDirs(result, subdir);
            }
        };
        ListAllTopLevelDirs(files, *filesystem);

        return files;
    }

    void ReadOnlyFilesystem::EmplaceContent(const SysPath& path, const std::string& error, BaseDirCallback&& callback) {
        bool result{};
        WalkDirectories(*filesystem, path, [&](Directory& target, const SysPath& directory) {
            if (directory == path.parent_path())
                result = callback(target, directory);
            return result;
        });

        if (!result)
            throw runtime_exception(error);
    }

    void ReadOnlyFilesystem::AddDirectory(const SysPath& path) {
        if (!filesystem) {
            filesystem.emplace([] {
                Directory root{};
                root.subdirs.emplace("/", Directory{});
                return root;
            }());
        }
        if (path.empty())
            return;

        EmplaceContent(path, "Nonexistent subdirectory, unable to create the new directory", [&](Directory& target, [[maybe_unused]] const SysPath& directory) {
            target.subdirs.emplace(path.filename(), Directory{});
            return true;
        });
    }

    void ReadOnlyFilesystem::AddFile(const SysPath& path, const FileBackingPtr& file) {
        EmplaceContent(path, "Failed to add the file", [&](Directory& target, const SysPath& directory) {
            target.files.emplace(directory / path.filename(), file);
            return true;
        });
    }
}
