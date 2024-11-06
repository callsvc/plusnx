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
        if (!ContainsValue(ListAllFiles(), path / filename))
            return {};
        return std::make_shared<RegularFile>(path / filename, mode);
    }

    std::vector<SysPath> RigidDirectory::ListAllFiles() const {
        std::vector<SysPath> content;
        std::function<void(const SysPath&)> DiscoverDirectory = [&](const SysPath& subdir) {
            std::filesystem::directory_iterator walker{subdir};
            if (walker == std::filesystem::directory_iterator{})
                return;

            for (const auto& entry : walker) {
                if (is_directory(entry)) {
                    DiscoverDirectory(entry);
                    continue;
                }
                content.emplace_back(entry);
            }
        };
        DiscoverDirectory(path);
        return content;
    }

    FileBackingPtr RigidDirectory::CreateFile(const SysPath& file) {
        assert(file.parent_path() == path);
        if (exists(file))
            return OpenFile(file, FileMode::Write);

        RegularFile createFile(file, FileMode::Write);
        return OpenFile(file, FileMode::Write);
    }

    void RigidDirectory::UnlikeFile(const SysPath& file) {
        assert(exists(file));
        std::filesystem::remove(file);
    }
}
