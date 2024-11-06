#include <ranges>
#include <boost/align/align_up.hpp>

#include <sys_fs/layered_fs.h>
#include <sys_fs/nx/readonly_filesystem.h>
namespace Plusnx::SysFs::Nx {
    ReadOnlyFilesystem::ReadOnlyFilesystem(const FileBackingPtr& romfs) {
        if (romfs->Read(content) != sizeof(RomFsHeader))
            return;
        assert(sizeof(RomFsHeader) == content.size);

        SysPath root{"/"};
        VisitSubdirectories(romfs, root, content.dirMetaOffset);

        const auto filesTable(romfs->GetBytes<u32>(content.dirHashSize / sizeof(u32), content.dirHashOffset));
        const auto countOfFiles = [&] {
            u32 count{};
            for (const auto& file : filesTable)
                if (file != RomFsEmptyEntry)
                    count++;
            return count;
        }();

        assert(ListAllFiles().size() == countOfFiles);
    }

    void AppendEntryName(const FileBackingPtr& romfs, SysPath& path, const u64 length, const u64 offset) {
        const auto dirName{romfs->GetBytes<char>(length, boost::alignment::align_up(offset, 4))};
        path.append(std::string_view(dirName.data(), dirName.size()));
    }

    void ReadOnlyFilesystem::VisitSubdirectories(const FileBackingPtr& romfs, SysPath& path, u64 offset) {
        auto PopulateSubdirectory = [&] (const DirectoryEntryMeta& entry) {
            if (entry.nameLength) {
                AppendEntryName(romfs, path, entry.nameLength, offset + sizeof(entry));
                AddDirectory(path);
            }
            if (entry.childDirOffset != RomFsEmptyEntry) {
                VisitSubdirectories(romfs, path, offset + entry.childDirOffset);
            }

            if (entry.firstFileOffset != RomFsEmptyEntry)
                VisitFiles(romfs, path, content.fileMetaOffset + entry.firstFileOffset);
        };
        DirectoryEntryMeta entry{};
        do {
            romfs->Read(entry, offset);

            PopulateSubdirectory(entry);
            if (path.has_parent_path())
                path = path.parent_path();

            offset += entry.nextDirSiblingOffset - sizeof(entry);
        } while (entry.nextDirSiblingOffset != RomFsEmptyEntry);
    }

    void ReadOnlyFilesystem::VisitFiles(const FileBackingPtr& romfs, SysPath& path, u64 offset) {
        FileEntryMeta file{};
        do {
            romfs->Read(file, offset);
            AppendEntryName(romfs, path, file.nameLength, offset + sizeof(file));

            AddFile(path, std::make_shared<FileLayered>(romfs, path, content.fileDataOffset + file.dataOffset, file.size));
            path = path.parent_path();

            offset += file.nextFileSiblingOffset - sizeof(file);

        } while (file.nextFileSiblingOffset != RomFsEmptyEntry);
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

        WalkDirectories(filesystem->second, path.parent_path(), OpenFileWithin);
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
        ListAllTopLevelDirs(files, filesystem->second);

        return files;
    }

    void ReadOnlyFilesystem::EmplaceContent(const SysPath& path, const std::string& error, BaseDirCallback&& callback) {
        bool result{};
        WalkDirectories(filesystem->second, path, [&](Directory& target, const SysPath& directory) {
            if (directory == path.parent_path())
                result = callback(target, directory);
            return result;
        });

        if (!result)
            throw Except("{}", error);
    }

    void ReadOnlyFilesystem::AddDirectory(const SysPath& path) {
        if (!filesystem) {
            filesystem.emplace(".", [&] {
                Directory placeholder{};
                placeholder.subdirs.emplace("/", Directory{});
                return placeholder;
            }());
        }
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
