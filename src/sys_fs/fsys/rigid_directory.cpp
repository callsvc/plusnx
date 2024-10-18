#include <functional>
#include <sys_fs/fsys/rigid_directory.h>
namespace Plusnx::SysFs::Fsys {
    std::vector<SysPath> RigidDirectory::ListAllFiles() {
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
        return content;
    }
}
