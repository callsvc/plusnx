#pragma once
#include <loader/app_loader.h>

#include <security/keyring.h>
#include <sys_fs/nx/content_metadata.h>
#include <sys_fs/nx/nca_core.h>
#include <sys_fs/nx/submission_package.h>
namespace Plusnx::Loader {
    class EShopTitle final : public AppLoader {
    public:
        EShopTitle(const std::shared_ptr<Security::Keyring>& _keys, const SysFs::FileBackingPtr& _nsp);

        bool ExtractFilesInto(const SysFs::SysPath& path) const override;
        std::optional<ProcessLoadResult> Load(std::shared_ptr<Core::Context>& context) override;
        std::shared_ptr<SysFs::Nx::ReadOnlyFilesystem> GetRomFs(bool isControl) const override;
        SysFs::FileBackingPtr GetNpdm() const override;

        std::unique_ptr<SysFs::Nx::SubmissionPackage> nsp;

        std::shared_ptr<SysFs::Nx::PartitionFilesystem> exefs;
        std::shared_ptr<SysFs::Nx::ReadOnlyFilesystem> romfs;
        std::shared_ptr<SysFs::Nx::ReadOnlyFilesystem> control;
    private:
        void GetAllContent();

        std::optional<SysFs::SysPath> VerifyTitleIntegrity() const;
    };
}
