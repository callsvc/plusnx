#pragma once
#include <filesystem>
#include <memory>

#include <sys_fs/provider.h>
#include <sys_fs/fsys/rigid_directory.h>
namespace Plusnx::Core {
    class Context;
}

namespace Plusnx::SysFs {
    class Assets {
        public:
        explicit Assets(const std::shared_ptr<Core::Context>& context);
        void CheckDir(const SysPath& path) const;
        void ResetDir(FSys::RigidDirectory& directory) const;

        FSys::RigidDirectory temp;
        FSys::RigidDirectory logs;
        FSys::RigidDirectory games;
        FSys::RigidDirectory keys;
        FSys::RigidDirectory nand;
        FSys::RigidDirectory cache;

        std::shared_ptr<Provider> provider;
    };
}