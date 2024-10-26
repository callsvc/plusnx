#include <loader/eshop_title.h>
namespace Plusnx::Loader {
    EShopTitle::EShopTitle(const SysFs::FileBackingPtr& nsp) :
        AppLoader(AppType::Nsp, ConstMagic<u32>("PFS0")),
        pfs(std::make_unique<SysFs::Nx::PartitionFilesystem>(nsp)) {

        if (!CheckHeader(nsp) || status != LoaderStatus::None)
            return;

        [[maybe_unused]] auto files{pfs->ListAllFiles()};
    }

    void EShopTitle::Load(std::shared_ptr<Core::Context>& process) {
        assert(pfs->ListAllFiles().size());
    }
}
