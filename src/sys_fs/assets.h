#pragma once
#include <filesystem>
#include <memory>

#include <sys_fs/provider.h>
#include <sys_fs/fsys/rigid_directory.h>
namespace Plusnx::Core {
    struct Context;
}

namespace Plusnx::SysFs {
    enum class SystemPaths {
        BaseDirectoryFs
    };

    enum class PlusnxDirectoryType {
        ConfigFile,
        ConfigFileBackup,
        Database,
        Telemetry
    };

    class Assets {
        public:
        explicit Assets(const std::shared_ptr<Core::Context>& context);
        void CheckDir(const SysPath& path) const;
        void ResetDir(FSys::RigidDirectory& directory) const;

        SysPath GetPlusnxFilename(PlusnxDirectoryType type) const;

        SysPath GetSystemPath(SystemPaths tagged) const;

        bool IsFromSystemPath(const SysPath& path) const;

        FSys::RigidDirectory temp;
        FSys::RigidDirectory user;
        FSys::RigidDirectory logs;
        FSys::RigidDirectory games;
        FSys::RigidDirectory keys;
        FSys::RigidDirectory nand;
        FSys::RigidDirectory cache;

        std::shared_ptr<Provider> provider;
    };
}