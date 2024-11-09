#include <generic_kernel/user_space.h>
#include <os/nx_sys.h>

#include <os/make_loader.h>
namespace Plusnx::Os {
    using AddrType = GenericKernel::AddressSpaceType;

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

        application = MakeLoader(context, backing);
        if (!application) {
            backing.reset();
            return;
        }

        if (const auto& process{context->process}) {
            const auto& npdm{process->npdm};
#if 1
            assert(npdm.environment == AddrType::Guest64Bit);
            npdm.DisplayBinaryInformation();
#endif
            process->us->CreateProcessMemory(npdm.environment);
        }

        creator.emplace(*this);
        creator->Initialize();
    }
}
