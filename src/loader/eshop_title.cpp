#include <ranges>
#include <print>

#include <security/checksum.h>
#include <sys_fs/fsys/rigid_directory.h>
#include <sys_fs/fsys/regular_file.h>
#include <sys_fs/nx/readonly_filesystem.h>

#include <details/perf_measure.h>
#include <sys_fs/extra/nso_modules.h>

#include <loader/game_fs.h>
#include <loader/eshop_title.h>
namespace Plusnx::Loader {
    EShopTitle::EShopTitle(const std::shared_ptr<Security::Keyring>& _keys, const SysFs::FileBackingPtr& _nsp) :
        AppLoader(AppType::Nsp, ConstMagic<u32>("PFS0")),
        nsp(std::make_unique<SysFs::Nx::SubmissionPackage>(_keys, _nsp)) {

        if (!CheckHeader(_nsp) || status != LoaderStatus::None)
            return;

#if 1
        if (const auto damaged = VerifyTitleIntegrity())
            throw exception("The NSP file is apparently corrupted, damaged file: {}", damaged->string());
#endif

        if (nsp)
            GetAllContent();

        titleId = nsp->GetProgramTitleId();
    }

    bool EShopTitle::ExtractFilesInto(const SysFs::SysPath& path) const {
        const auto gameFs{std::make_shared<SysFs::FSys::RigidDirectory>(path, true)};
        for (const auto& dirpath : {"exefs", "romfs", "control"}) {
            if (const auto directory{gameFs->CreateSubDirectory(dirpath)}; !directory)
                throw exception("Failed to create directory: {}", SysFs::SysPath{gameFs->path / dirpath}.string());
        }

        exefs->ExtractAllFiles(gameFs->path / "exefs");
        romfs->ExtractAllFiles(gameFs->path / "romfs");
        if (control)
            control->ExtractAllFiles(gameFs->path / "control");

        const auto nspExtracted{std::make_unique<GameFs>(std::move(gameFs))};
        nspExtracted->RegenerateGfs();

        return true;
    }

    std::optional<ProcessLoadResult> EShopTitle::Load(std::shared_ptr<Core::Context>& context) {
        if (!exefs)
            return {};

        const auto& process{context->process};
        auto modules{exefs->ListAllFiles()};

        modules.erase(std::ranges::find(modules, "main.npdm"));

        SysFs::Extra::NsoModules nsoHolder;
        const auto files{nsoHolder.OrderExecutableFiles(exefs)};

        u64 startsAddr{process->creation->codeAddr};

        // Do not allocate the process, just to verify how the memory layout will be
        if (const auto [base, size] = nsoHolder.LoadProgramImage(process, startsAddr, files, false); base && size) {
            assert(base + size == startsAddr);

            process->creation->codeNumPages = size / Nxk::SwitchPageSize;
        }

        process->mm->CreateProcessMemory(process);

        startsAddr = process->creation->codeAddr;
        if (const auto [entryPoint, allocatedSize] = nsoHolder.LoadProgramImage(process, startsAddr, files); allocatedSize) {
            assert(process->creation->codeNumPages * Nxk::SwitchPageSize == allocatedSize);
            process->entry = reinterpret_cast<void*>(process->mm->code.data() + entryPoint);
            return ProcessLoadResult{entryPoint, allocatedSize};
        }

        return {};
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
                if (auto partition{std::make_shared<SysFs::Nx::PartitionFilesystem>(backingNcaFile)}; IsAExeFsPartition(partition))
                    exefs = std::move(partition);
            }
        }
        if (const auto ncaCtrl{nsp->GetIndexedNcas(SysFs::Nx::ContentType::Control, metaType)}; !ncaCtrl.empty())
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
        const Details::PerfMeasure measure{Details::Metrics::Milli};

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
        std::println("Verified data size: {} - Execution time: {}", SysFs::GetReadableSize(totalBytes), measure.GetElapsed());
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
