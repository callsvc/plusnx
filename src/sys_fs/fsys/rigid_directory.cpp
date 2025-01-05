#include <functional>
#include <sys_fs/fsys/rigid_directory.h>
#include <sys_fs/fsys/regular_file.h>
namespace Plusnx::SysFs::FSys {
    RigidDirectory::RigidDirectory(const SysPath& path, const bool create) : DirectoryBacking(path) {
        if (create) {
            if (path.has_parent_path())
                create_directories(path);
            else
                create_directory(path);
        }
    }

    FileBackingPtr RigidDirectory::OpenFile(const SysPath& filename, const FileMode mode) {
        auto filepath{filename};
        if (!filepath.has_parent_path())
            filepath = path / filepath;

        if (mode == FileMode::Read)
            if (!Exists(filepath))
                return {};
        return std::make_shared<RegularFile>(filepath, mode, true);
    }

    std::vector<SysPath> RigidDirectory::ListAllFiles() const {
        std::vector<SysPath> content;
        std::function<void(const SysPath&)> DiscoverDirectory = [&](const SysPath& subdir) {
            std::filesystem::directory_iterator walker{subdir};
            if (walker == std::filesystem::directory_iterator{})
                return;

            for (const auto& entry : walker) {
                if (is_directory(entry) && !is_symlink(entry)) {
                    DiscoverDirectory(entry);
                    continue;
                }
                content.emplace_back(entry);
            }
        };
        DiscoverDirectory(path);
        return content;
    }
    std::shared_ptr<RigidDirectory> RigidDirectory::CreateSubDirectory(const SysPath& dirname) const {
        auto dirpath{dirname};
        if (!dirpath.has_parent_path())
            dirpath = path / dirpath;

        if (!is_directory(dirpath))
            return {};

        return std::make_shared<RigidDirectory>(dirpath, true);
    }

    FileBackingPtr RigidDirectory::CreateFile(const SysPath& file) {
        auto filepath{file};
        if (!filepath.has_parent_path() || filepath.parent_path() != path)
            filepath = path / filepath;

        if (exists(filepath))
            return OpenFile(filepath, FileMode::Write);

        RegularFile createFile(filepath, FileMode::Write);
        return OpenFile(filepath, FileMode::Write);
    }

    void RigidDirectory::UnlinkFile(const SysPath& file) {
        assert(exists(file));

        if (RegularFile _openedFile{file, FileMode::Write, true})
            ShredderFile(_openedFile);

        std::filesystem::remove(file);
    }
}
