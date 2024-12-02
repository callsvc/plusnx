#pragma once

#include <sys_fs/fs_types.h>
namespace Plusnx::Core {
    class Configs {
    public:
        Configs() : ence(), excfs() {}
        void Initialize(const SysFs::SysPath& path);
        void ExportConfigs(const SysFs::SysPath& output) const;

        std::vector<i32> affinity;

        bool ence;
        bool excfs;
    };
}