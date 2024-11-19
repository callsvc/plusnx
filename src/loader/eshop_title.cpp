#include <ranges>
#include <print>

#include <security/checksum.h>
#include <sys_fs/fsys/rigid_directory.h>
#include <sys_fs/fsys/regular_file.h>
#include <sys_fs/nx/readonly_filesystem.h>

#include <details/perf_measure.h>
#include <loader/eshop_title.h>
#include <sys_fs/ext/nso_modules.h>
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

    void EShopTitle::Load(std::shared_ptr<Core::Context>& context) {
        if (!exefs)
            return;

        [[maybe_unused]] const auto& process{context->process};
        auto modules{exefs->ListAllFiles()};

        constexpr u64 baseAddr{};
        modules.erase(std::ranges::find(modules, "main.npdm"));

        SysFs::Ext::NsoModules nsoHolder;
        std::vector<SysFs::FileBackingPtr> files;

        // We need to maintain the order of these objects in memory layout
        std::vector<SysFs::SysPath> modulesArray{"rtld", "sdk"};
        for (u32 sub{}; sub <= 6; sub++)
            modulesArray.emplace_back(std::format("subsdk{}", sub));

        for (const auto& target : modulesArray) {
            if (ContainsValue(modules, target)) {
                files.emplace_back(exefs->OpenFile(target));
            }
        }

        nsoHolder.LoadProgramImage(baseAddr, files);
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
        if (const auto ncaCtrl = nsp->GetIndexedNcas(SysFs::Nx::ContentType::Control, metaType); !ncaCtrl.empty())
            for (const auto& [type, file] : ncaCtrl.front()->GetBackingFiles())
                if (type == SysFs::Nx::FsType::RomFs)
                    control = std::make_unique<SysFs::Nx::ReadOnlyFilesystem>(file);
    }

    std::optional<SysFs::SysPath> EShopTitle::VerifyTitleIntegrity() const {
        Security::Checksum checksum;

        std::array<u8, 32> result;
        std::vector<u8> buffer(4 * 1024 * 1024);

        const auto files{nsp->GetAllNcas()};

        u64 totalBytes{};
        Details::PerfMeasure measure{Details::Metrics::Milli};

        std::print("Performing a checksum on eligible NCAs...\n");
        for (const auto& nca : files) {
            std::array<u8, 16> expected;
            u64 processed{};
            if (!nca->VerifyNca(expected, checksum, buffer, processed))
                continue;
            totalBytes += processed;

            checksum.Finish(std::span(result));

            if (!IsEqual(std::span(result).subspan(0, 16), std::span(expected)))
                return nca->path;
        }

        double elapsed;
        measure.Stop(elapsed);
        std::print("Verified data size: {} - Execution time: {}ms\n", SysFs::GetReadableSize(totalBytes), elapsed);

        return {};
    }

    std::shared_ptr<SysFs::Nx::ReadOnlyFilesystem> EShopTitle::GetRomFs(const bool isControl) const {
        if (isControl)
            return control;
        return {};
    }
    SysFs::FileBackingPtr EShopTitle::GetNpdm() const {
        if (IsAExeFsPartition(exefs))
            return exefs->OpenFile("main.npdm");
        return {};
    }
}
