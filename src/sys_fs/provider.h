#pragma once

#include <unordered_map>

#include <types.h>
#include <sys_fs/fs_types.h>
namespace Plusnx::SysFs {

    constexpr std::string rootPathId{"system"};
    class Provider {
    public:
        Provider();
        [[nodiscard]] SysPath GetRoot() const;

        void RegisterSystemPath(const SysPath& directory);
    private:
        std::unordered_map<std::string, SysPath> dirs;
        u64 opened{};
        u64 fileMax{};
    };
}
