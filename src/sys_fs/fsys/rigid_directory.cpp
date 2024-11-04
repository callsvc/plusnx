#include <functional>
#include <sys_fs/fsys/rigid_directory.h>
#include <sys_fs/fsys/regular_file.h>
namespace Plusnx::SysFs::FSys {
    RigidDirectory::RigidDirectory(const SysPath& path, const bool create) : RoDirectoryBacking(path) {
        if (create) {
            if (path.has_parent_path())
                create_directories(path);
            else
                create_directory(path);
        }
    }

    FileBackingPtr RigidDirectory::OpenFile(const SysPath& path) {
        if (!ContainsValue(ListAllFiles(), path))
            return {};
        return std::make_shared<FSys::RegularFile>(path);
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
}
