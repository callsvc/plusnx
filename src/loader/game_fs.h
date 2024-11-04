#pragma once
#include <sys_fs/fsys/rigid_directory.h>
#include <loader/app_loader.h>

namespace Plusnx::Loader {
    class GameFs final : public AppLoader {
    public:
        GameFs(const SysFs::FSys::RigidDirectory& directory);
    };
}
