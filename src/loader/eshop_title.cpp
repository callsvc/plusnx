#include <ranges>
#include <print>

#include <security/checksum.h>
#include <sys_fs/fsys/rigid_directory.h>
#include <sys_fs/fsys/regular_file.h>
#include <sys_fs/nx/readonly_filesystem.h>

#include <sys_fs/nx/nso_core.h>
#include <loader/eshop_title.h>
namespace Plusnx::Loader {
    EShopTitle::EShopTitle(const std::shared_ptr<Security::Keyring>& _keys, const SysFs::FileBackingPtr& _nsp) :
        AppLoader(AppType::Nsp, ConstMagic<u32>("PFS0")),
        nsp(std::make_unique<SysFs::Nx::SubmissionPackage>(_keys, _nsp)) {

        if (!CheckHeader(_nsp) || status != LoaderStatus::None)
            return;

#if 1
        if (const auto damaged = VerifyTitleIntegrity())
            throw runtime_plusnx_except("The NSP file is apparently corrupted, damaged file: {}", damaged->string());
#endif

        if (nsp)
            GetAllContent();
    }

    bool EShopTitle::ExtractFilesInto(const SysFs::SysPath& path) const {
        SysFs::FSys::RigidDirectory gameFs(path, true);
        return true;
    }

    void EShopTitle::Load(std::shared_ptr<Core::Context> &context) {
        if (!exefs)
            return;
        if (const auto npdm{exefs->OpenFile("main.npdm")}; !npdm) {
            throw runtime_plusnx_except("The NSP does not have a valid ExeFS, preventing it from loading");
        }
        const auto &process{context->process};
        process->npdm = SysFs::Npdm(exefs->OpenFile("main.npdm"));

        [[maybe_unused]] SysFs::Nx::NsoCore main(exefs->OpenFile("main"));
    }

    void EShopTitle::GetAllContent() {
        constexpr auto metaType{SysFs::Nx::ContentMetaType::Application};
        const auto ncas{nsp->GetIndexedNcas(SysFs::Nx::ContentType::Program, metaType)};

        for (const auto& _nca : ncas) {
            for (const auto& [type, backingNcaFile] : _nca->GetBackingFiles()) {
                if (type == SysFs::Nx::FsType::RomFs) {
                    romfs = std::make_shared<SysFs::Nx::ReadOnlyFilesystem>(backingNcaFile);
                    continue;
                }
                auto partition{std::make_unique<SysFs::Nx::PartitionFilesystem>(backingNcaFile)};
                if (IsAExeFsPartition(partition))
                    exefs = std::move(partition);
            }
        }
        const auto controlNca{nsp->GetIndexedNcas(SysFs::Nx::ContentType::Control, metaType)};
        if (!controlNca.empty())
            for (const auto& [type, file] : controlNca.front()->GetBackingFiles())
                if (type == SysFs::Nx::FsType::RomFs)
                    control = std::make_unique<SysFs::Nx::ReadOnlyFilesystem>(file);
    }

    std::optional<SysFs::SysPath> EShopTitle::VerifyTitleIntegrity() const {
        Security::Checksum checksum;

        std::array<u8, 32> result;
        std::vector<u8> buffer(4 * 1024 * 1024);

        const auto files{nsp->GetAllNcas()};

        for (const auto& nca : files) {
            std::array<u8, 16> expected;
            if (!nca->VerifyNca(expected, checksum, buffer))
                continue;

            checksum.Finish(std::span(result));

            if (!IsEqual(std::span(result).subspan(0, 16), std::span(expected)))
                return nca->path;
        }
        return {};
    }
}
