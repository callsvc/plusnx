#pragma once
#include <sys_fs/nx/nca_core.h>
#include <loader/app_loader.h>

namespace Plusnx::Loader {
    class ContentArchive final : public AppLoader {
    public:
        ContentArchive(const std::shared_ptr<Security::Keyring>& keys, const SysFs::FileBackingPtr& nca);

        void Load(std::shared_ptr<Core::Context>& context) override;
    private:
        std::unique_ptr<SysFs::Nx::NcaCore> nca;
    };
}