#include <generic_kernel/user_space.h>
#include <generic_kernel/svc/parameter_types.h>

#include <os/make_loader.h>
#include <os/nx_sys.h>
namespace Plusnx::Os {
    using AddrType = GenericKernel::AddressSpaceType;

    void NxSys::CheckNpdmPresence() const {
        const auto exefs{application->GetNpdm()};
        const auto type{application->type};
        if (application->type == Loader::AppType::Nsp ||
            application->type == Loader::AppType::Xci ||
            application->type == Loader::AppType::GameFs) {

            if (!exefs)
                throw runtime_exception("The {} does not have a valid ExeFS, preventing it from loading", Loader::GetTypeName(type));
        }
    }

    void NxSys::PopulateProcessParameters(const SysFs::MetaProgram& npdm) const {
        GenericKernel::Svc::CreateProcessParameter creation{};
        npdm.Populate(creation);

        const auto& process{context->process};
        process->creation.emplace(creation);
    }

    void NxSys::LoadApplicationFile(const SysFs::SysPath& path) {
        assert(std::filesystem::exists(path));
        std::error_code err;

        backing = [&] -> SysFs::FileBackingPtr {
            if (context->assets->IsFromSystemPath(path)) {
                const auto target{absolute(path, err)};
                if (!err.value())
                    return context->provider->OpenSystemFile(SysFs::RootId, target);
            }
            return std::make_shared<SysFs::FSys::RegularFile>(path);
        }();

        application = MakeLoader(context, backing);
        if (!application) {
            backing.reset();
            return;
        }

        if (const auto& process{context->process}) {
            CheckNpdmPresence();

            process->npdm = SysFs::MetaProgram(application->GetNpdm());
            const auto& npdm{process->npdm};
#if 1
            if (context->configs->ence)
                context->configs->ence = npdm.addressType == GenericKernel::AddressSpaceType::AddressSpace64Bit;
            assert(npdm.addressType == AddrType::AddressSpace64Bit);
            npdm.DisplayBinaryInformation();

            PopulateProcessParameters(npdm);
#endif
            assert(process->creation);
        }

        creator.emplace(*this);
        creator->Initialize();

        context->process->AllocateTlsHeapRegion();

        context->details.emplace(creator->GetQolGame());
    }
}
