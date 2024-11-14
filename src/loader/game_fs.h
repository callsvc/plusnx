#pragma once
#include <sys_fs/fsys/rigid_directory.h>
#include <loader/app_loader.h>

namespace Plusnx::Loader {
    class GameFs final : public AppLoader {
    public:
        GameFs(const SysFs::FSys::RigidDirectory &directory);
        void Load(std::shared_ptr<Core::Context> &context) override;
        std::shared_ptr<SysFs::Nx::ReadOnlyFilesystem> GetRomFs(bool isControl) const override;
    };
}
