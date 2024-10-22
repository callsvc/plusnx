#include <filesystem>
#include <fstream>

#include <sys_fs/provider.h>
#include <sys_fs/fsys/rigid_directory.h>
namespace Plusnx::SysFs {
    Provider::Provider() {
        const std::filesystem::directory_iterator descriptors("/proc/self/fd");
        for (const auto& file : descriptors) {
            assert(file.is_symlink());
            opened++;
        }

        std::fstream barrier("/proc/sys/fs/file-max", std::ios::in);
        barrier >> fileMax;
    }

    SysPath Provider::GetRoot() const {
        if (dirs.contains(RootId))
            return dirs.at(RootId).front();
        return std::filesystem::current_path();
    }

    void Provider::RegisterSystemPath(const std::string& card, const SysPath& directory) {
        assert(directory.has_root_directory());
        if (!exists(directory))
            if (!create_directory(directory))
                if (exists(directory.parent_path()))
                    assert(0);

        auto& paths{dirs[card]};
        paths.emplace_back(directory);
    }

    RegularFilePtr Provider::OpenSystemFile(const std::string& card, const SysPath& fullpath) {
        for (std::weak_ptr<FSys::RegularFile> file : cachedFiles) {
            if (auto aliveFile = file.lock())
                if (aliveFile->path == fullpath)
                    return aliveFile;
        }
        if (!dirs.contains(card))
            return {};
        for (const auto& directory : dirs[card]) {
            const Fsys::RigidDirectory dir(directory);

            for (const auto& entry : dir.ListAllFiles()) {
                if (entry != fullpath)
                    continue;

                cachedFiles.emplace_back(std::make_shared<FSys::RegularFile>(fullpath));
                return cachedFiles.back();
            }
        }
        return {};
    }
}
