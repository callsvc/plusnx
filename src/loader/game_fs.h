#pragma once
#include <sys_fs/fsys/rigid_directory.h>
#include <loader/app_loader.h>

namespace Plusnx::Loader {
    class GameFs final : public AppLoader {
    public:
        GameFs(const std::shared_ptr<SysFs::FSys::RigidDirectory>& directory);

        void RegenerateGfs() const;

        const std::shared_ptr<SysFs::FSys::RigidDirectory> backing;
        void Load(std::shared_ptr<Core::Context>& context) override;
    };
}
