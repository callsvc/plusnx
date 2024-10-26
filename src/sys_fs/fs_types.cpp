#include <sys_fs/fs_types.h>

#include <sys_fs/fsys/regular_file.h>

namespace Plusnx::SysFs {
    bool FileSystem::WalkDirectories(Directory& directory, SysPath& iterator, const SysPath& target, BaseDirCallback&& callback) {
        for (auto& [path, dir] : directory.subdirs) {
            iterator += path;
            if (WalkDirectories(dir, iterator, target, std::move(callback)))
                return true;
            if (callback(dir, iterator))
                return true;
            iterator = iterator.parent_path();
        }
        return callback(directory, iterator);
    }

    void FileSystem::ExtractAllFiles(const SysPath& output) {
        const auto cachedFiles{ListAllFiles()};
        for (const auto& filename : cachedFiles) {
            const auto file{OpenFile(filename)};
            const auto outputFile(std::make_unique<FSys::RegularFile>(output / filename.relative_path(), FileMode::Write));

            auto content(file->GetBytes(file->GetSize()));
            outputFile->WriteBytes(content);
        }
    }
}
