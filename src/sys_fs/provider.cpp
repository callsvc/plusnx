#include <filesystem>
#include <fstream>

#include <sys_fs/provider.h>
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

    std::filesystem::path Provider::GetRoot() const {
        if (dirs.contains(rootPathId))
            return dirs.at(rootPathId);
        return std::filesystem::current_path();
    }

    void Provider::RegisterSystemPath(const std::filesystem::path& directory) {
        assert(directory.has_root_directory());
        if (!exists(directory))
            if (!create_directory(directory))
                if (exists(directory.parent_path()))
                    assert(0);

        dirs.emplace(directory.parent_path(), directory);
    }
}
