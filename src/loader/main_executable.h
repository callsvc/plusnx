#pragma once
#include <sys_fs/nx/nso_core.h>
#include <loader/app_loader.h>
namespace Plusnx::Loader {
    class MainExecutable final : public ExecutableAppLoader {
    public:
        MainExecutable(const SysFs::FileBackingPtr& nso);
        void Load(std::shared_ptr<Core::Context>& context) override;

    private:
        std::unique_ptr<SysFs::Nx::NsoCore> nso;
    };
}