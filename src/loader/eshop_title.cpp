#include <loader/eshop_title.h>
namespace Plusnx::Loader {
    EShopTitle::EShopTitle(const SysFs::RegularFilePtr& nsp) :
        AppLoader(AppType::Nsp, ConstMagic<u32>("PFS0")),
        pfs(std::make_unique<SysFs::Nx::PartitionFilesystem>(nsp)) {

        if (!CheckHeader(nsp))
            return;
    }
}
