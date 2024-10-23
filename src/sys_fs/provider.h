#pragma once
#include <unordered_map>

#include <sys_fs/fs_types.h>
#include <sys_fs/fsys/regular_file.h>
namespace Plusnx::SysFs {

    constexpr std::string RootId{"Root"};
    class Provider {
    public:
        Provider();
        [[nodiscard]] SysPath GetRoot() const;

        void RegisterSystemPath(const std::string& card, const SysPath& directory);
        FileBackingPtr OpenSystemFile(const std::string& card, const SysPath& fullpath);

        std::unordered_map<std::string, std::vector<SysPath>> dirs;
        std::vector<FileBackingPtr> cachedFiles;
    private:
        u64 opened{};
        u64 fileMax{};
    };
}
