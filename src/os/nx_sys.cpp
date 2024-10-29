#include <loader/eshop_title.h>
#include <loader/nx_executable.h>

#include <generic_kernel/user_space.h>
#include <os/nx_sys.h>
namespace Plusnx::Os {
    using AddrType = GenericKernel::AddressSpaceType;

    Loader::AppType GetAppTypeByFilename(const SysFs::SysPath& filename) {
        if (filename.extension() == ".nsp")
            return Loader::AppType::Nsp;
        if (filename.extension() == ".xci")
            return Loader::AppType::Xci;
        if (filename.extension() == ".nro")
            return Loader::AppType::Nro;

        return Loader::AppType::Invalid;
    }

    void NxSys::LoadApplicationFile(const SysFs::SysPath& path) {
        assert(std::filesystem::exists(path));
        std::error_code err;

        backing = [&] -> SysFs::FileBackingPtr {
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
        application = [&] -> std::shared_ptr<Loader::AppLoader> {
            switch (type) {
                case Loader::AppType::Nsp:
                case Loader::AppType::Xci:
                    return std::make_shared<Loader::EShopTitle>(backing);
                case Loader::AppType::Nro:
                    return std::make_shared<Loader::NxExecutable>(backing);
                default:
                    return nullptr;
            }
        }();

        if (const auto& process{context->process.lock()}) {
            const auto& npdm{process->npdm};
#if 1
            assert(npdm.environment == AddrType::Guest64Bit);
            npdm.DisplayBinaryInformation();
#endif
            process->memory->CreateProcessMemory(npdm.environment);
        }

        creator.emplace(*this);
        creator->Initialize();
    }
}
