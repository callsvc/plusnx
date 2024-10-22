#include <loader/eshop_title.h>
#include <core/context.h>

#include <os/nx_sys.h>
namespace Plusnx::Os {
    Loader::AppType GetAppTypeByFilename(const SysFs::SysPath& filename) {
        if (filename.extension() == ".nsp")
            return Loader::AppType::Nsp;
        if (filename.extension() == ".xci")
            return Loader::AppType::Xci;

        return Loader::AppType::Invalid;
    }

    void NxSys::LoadApplicationFile(const SysFs::SysPath& path) {
        assert(std::filesystem::exists(path));
        std::error_code err;

        backing = [&] {
            if (context->IsFromSystemPath(path)) {
                const auto target{absolute(path, err)};
                if (!err.value())
                    return context->provider->OpenSystemFile(SysFs::RootId, target);
            }
            return std::make_shared<SysFs::FSys::RegularFile>(path);
        }();

        const auto type{GetAppTypeByFilename(path.filename())};
        if (type == Loader::AppType::Invalid) {
            backing.reset();
            return;
        }

        switch (type) {
            case Loader::AppType::Nsp:
                case Loader::AppType::Xci:
                application = std::make_shared<Loader::EShopTitle>(backing);
            default: {}
        }
    }
}
