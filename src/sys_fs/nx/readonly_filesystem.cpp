#include <ranges>
#include <boost/align/align_up.hpp>

#include <sys_fs/bounded.h>
#include <sys_fs/nx/readonly_filesystem.h>

namespace Plusnx::SysFs::Nx {
    std::function<bool(Directory&, SysPath&, const SysPath&, BaseDirCb&&)> FileSystemTraverser = [](auto& directory, auto& walker, const auto& target, auto&& callback) {
        for (auto& [dirName, dir] : directory.subdirs) {
            walker += dirName;
            if (FileSystemTraverser(dir, walker, target, std::move(callback)))
                return true;
            if (callback(dir, walker))
                return true;
            walker = walker.parent_path();
        }
        return callback(directory, walker);
    };

    ReadOnlyFilesystem::ReadOnlyFilesystem(const FileBackingPtr& romfs) {
        if (romfs->Read(header) != sizeof(RomFsHeader))
            return;
        assert(sizeof(RomFsHeader) == header.size);

        SysPath root{"/"};
        VisitSubdirectories(romfs, root, header.dirMetaOffset);
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
                VisitFiles(romfs, path, header.fileMetaOffset + entry.firstFileOffset);
        };
        bool process{};
        do {
            const auto entry{romfs->Read<DirectoryEntryMeta>(offset)};
            PopulateSubdirectory(entry);
            if (path.has_parent_path())
                path = path.parent_path();

            if ((process = entry.nextDirSiblingOffset != RomFsEmptyEntry))
                offset += entry.nextDirSiblingOffset - sizeof(entry);
            if (process)
                assert(offset < header.dirMetaOffset + header.dirMetaSize);
        } while (process);
    }

    void ReadOnlyFilesystem::VisitFiles(const FileBackingPtr& romfs, SysPath& path, u64 offset) {
        bool process{};
        do {
            const auto file{romfs->Read<FileEntryMeta>(offset)};
            AppendEntryName(romfs, path, file.nameLength, offset + sizeof(file));

            AddFile(path, std::make_shared<FileBounded>(romfs, path, header.fileDataOffset + file.dataOffset, file.size));
            path = path.parent_path();

            if ((process = file.nextFileSiblingOffset != RomFsEmptyEntry))
                offset += file.nextFileSiblingOffset;

        } while (process);
    }

    FileBackingPtr ReadOnlyFilesystem::OpenFile(const SysPath& path) {
        FileBackingPtr result;
        auto OpenFileWithin = [&](const Directory& directory, const auto& parent) {
            for (const auto& file : std::ranges::views::values(directory.files)) {
                if (file->path.parent_path() == parent)
                    if (file->path == path)
                        result = file;
            }
            return result != nullptr;
        };
        SysPath walker{};
        FileSystemTraverser(filesystem->second, walker, path.parent_path(), OpenFileWithin);
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

    void ReadOnlyFilesystem::AddDirectory(const SysPath& path) {
        if (!filesystem) {
            filesystem.emplace("romfs:", [&] {
                Directory placeholder{};
                placeholder.subdirs.emplace("/", Directory{});
                return placeholder;
            }());
        }
        bool result;
        SysPath walker{};

        FileSystemTraverser(filesystem->second, walker, path, [&](Directory& target, const SysPath& directory) {
            if ((result = directory == path.parent_path()))
                target.subdirs.emplace(path.filename(), Directory{});
            return result;
        });

        if (!result)
            throw std::runtime_error("Nonexistent subdirectory, unable to create the new directory");
    }

    void ReadOnlyFilesystem::AddFile(const SysPath& path, const FileBackingPtr& file) {
        SysPath walker{};
        bool result;

        FileSystemTraverser(filesystem->second, walker, path, [&](Directory& target, const SysPath& directory) {
            if ((result = directory == path.parent_path()))
                target.files.emplace(directory / path.filename(), file);
            return result;
        });

        if (!result)
            throw std::runtime_error("Failed to add the file");
    }
}
