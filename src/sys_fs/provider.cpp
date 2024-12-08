#include <filesystem>
#include <fstream>
#include <print>

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

    FileBackingPtr Provider::OpenSystemFile(const std::string& card, const SysPath& fullpath) {
        for (std::weak_ptr file : cachedFiles) {
            if (std::shared_ptr aliveFile{file})
                if (aliveFile->path == fullpath)
                    return aliveFile;
        }
        if (!dirs.contains(card))
            return {};
        for (const auto& directory : dirs[card]) {
            FSys::RigidDirectory dir(directory);

            if (auto file{dir.OpenFile(fullpath)}) {
                cachedFiles.emplace_back(file);
                return cachedFiles.back();
            }
        }
        return {};
    }

    FileBackingPtr Provider::CreateSystemFile(const std::string& card, const SysPath& fullpath) {
        for (const auto& directory : dirs[card]) {
            if (fullpath.parent_path() != directory)
                continue;
            FSys::RigidDirectory dir(directory);
            if (auto file{dir.CreateFile(fullpath)}; file != nullptr) {
                cachedFiles.emplace_back(file);
                return cachedFiles.back();
            }
        }
        return {};
    }

    void Provider::RemoveCachedFile(const SysPath& path) const {
        for (const auto& file : cachedFiles) {
            if (file->path.parent_path() == path) {
                std::print("Must be removed immediately: {}\n", file->GetPath());
            }
        }
    }
}
