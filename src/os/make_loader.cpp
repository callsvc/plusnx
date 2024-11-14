#include <loader/eshop_title.h>
#include <loader/nx_executable.h>
#include <loader/cartridge.h>
#include <loader/content_archive.h>
#include <loader/main_executable.h>

#include <os/make_loader.h>
namespace Plusnx::Os {
    Loader::AppType GetAppTypeByFilename(const SysFs::SysPath& filename) {
        if (filename.extension() == ".nsp")
            return Loader::AppType::Nsp;
        if (filename.extension() == ".xci")
            return Loader::AppType::Xci;
        if (filename.extension() == ".nca")
            return Loader::AppType::Nca;
        if (filename.extension() == ".nro")
            return Loader::AppType::Nro;
        if (filename.extension() == ".nso")
            return Loader::AppType::Nso;

        return Loader::AppType::Invalid;
    }

    std::shared_ptr<Loader::AppLoader> MakeLoader(const std::shared_ptr<Core::Context>& context, const SysFs::FileBackingPtr& file) {
        const auto type{GetAppTypeByFilename(file->path)};

        auto result = [&] -> std::shared_ptr<Loader::AppLoader> {
            switch (type) {
                case Loader::AppType::Nsp:
                    return std::make_shared<Loader::EShopTitle>(context->keys, file);
                case Loader::AppType::Xci:
                    return std::make_shared<Loader::Cartridge>(file);
                case Loader::AppType::Nca:
                    return std::make_shared<Loader::ContentArchive>(context->keys, file);
                case Loader::AppType::Nro:
                    return std::make_shared<Loader::NxExecutable>(file);
                case Loader::AppType::Nso:
                    return std::make_shared<Loader::MainExecutable>(file);
                default:
                    return nullptr;
            }
        }();

        return result;
    }
}
