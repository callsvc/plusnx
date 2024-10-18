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

        Fsys::RigidDirectory logs;
        Fsys::RigidDirectory games;
        Fsys::RigidDirectory keys;
        Fsys::RigidDirectory nand;
        Fsys::RigidDirectory cache;

        std::shared_ptr<Provider> provider;
    };
}