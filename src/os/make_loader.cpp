#include <os/make_loader.h>

#include <loader/eshop_title.h>
#include <loader/nx_executable.h>

namespace Plusnx::Os {
    Loader::AppType GetAppTypeByFilename(const SysFs::SysPath& filename) {
        if (filename.extension() == ".nsp")
            return Loader::AppType::Nsp;
        if (filename.extension() == ".xci")
            return Loader::AppType::Xci;
        if (filename.extension() == ".nro")
            return Loader::AppType::Nro;

        return Loader::AppType::Invalid;
    }

    std::shared_ptr<Loader::AppLoader> MakeLoader(const std::shared_ptr<Core::Context>& context, const SysFs::FileBackingPtr& file) {
        const auto type{GetAppTypeByFilename(file->path)};

        auto result = [&] -> std::shared_ptr<Loader::AppLoader> {
            switch (type) {
                case Loader::AppType::Nsp:
                case Loader::AppType::Xci:
                    return std::make_shared<Loader::EShopTitle>(context->keys, file);
                case Loader::AppType::Nro:
                    return std::make_shared<Loader::NxExecutable>(file);
                default:
                    return nullptr;
            }
        }();

        return result;
    }
}
