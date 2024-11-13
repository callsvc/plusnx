#include <ranges>
#include <print>

#include <security/checksum.h>
#include <sys_fs/fsys/rigid_directory.h>
#include <sys_fs/nx/readonly_filesystem.h>
#include <loader/eshop_title.h>
namespace Plusnx::Loader {
    EShopTitle::EShopTitle(const std::shared_ptr<Security::Keyring>& _keys, const SysFs::FileBackingPtr& nsp) :
        AppLoader(AppType::Nsp, ConstMagic<u32>("PFS0")),
        pfs(std::make_unique<SysFs::Nx::PartitionFilesystem>(nsp)),
        keys(_keys) {

        if (!CheckHeader(nsp) || status != LoaderStatus::None)
            return;

        const auto files{pfs->ListAllFiles()};
        for (const auto& file : files) {
            if (GetEntryFormat(file) == ContainedFormat::Ticket)
                ImportTicket(file);
        }

        std::optional<SysFs::Nx::NCA> cnmt;
#if 1
        if (const auto damaged = ValidateAllFiles(files)) {
            throw Except("The NSP file is apparently corrupted, damaged file: {}", damaged->string());
        }
#endif

        for (const auto& file : files) {
            const auto type{GetEntryFormat(file)};
            if (type == ContainedFormat::Nca)
                contents.emplace_back(std::make_unique<SysFs::Nx::NCA>(keys, pfs->OpenFile(file)));
            else if (type == ContainedFormat::Cnmt)
                cnmt.emplace(keys, pfs->OpenFile(file));
        }

        IndexNcaEntries(cnmt);
    }

    bool EShopTitle::ExtractFilesInto(const SysFs::SysPath& path) const {
        SysFs::FSys::RigidDirectory gameFs(path, true);
        return true;
    }

    void EShopTitle::Load(std::shared_ptr<Core::Context>& context) {
        if (!exefs)
            return;

        if (const auto npdm{exefs->OpenFile("main.npdm")}; !npdm) {
            throw Except("The NSP does not have a valid ExeFS, preventing it from loading");
        }
        const auto& process{context->process};
        process->npdm = SysFs::Npdm(exefs->OpenFile("main.npdm"));

    }
    void EShopTitle::ImportTicket(const SysFs::SysPath& filename) const {
        const auto tikFile{pfs->OpenFile(filename)};
        keys->AddTicket(std::move(std::make_unique<Security::Ticket>(tikFile)));
    }
    void EShopTitle::IndexNcaEntries([[maybe_unused]] const std::optional<SysFs::Nx::NCA>& metadata) {
        assert(contents.size());

        for (const auto& nextNca : contents) {
            for (const auto& [type, backingNcaFile] : nextNca->GetBackingFiles()) {
                if (type == SysFs::Nx::FsType::RomFs) {
                    if (!romfs)
                        romfs = std::make_shared<SysFs::Nx::ReadOnlyFilesystem>(backingNcaFile);
                    continue;
                }
                auto partition{std::make_unique<SysFs::Nx::PartitionFilesystem>(backingNcaFile)};
                if (IsAExeFsPartition(partition))
                    exefs = std::move(partition);
            }
        }
        if (romfs)
            if (const auto file = romfs->OpenFile("/control.nacp"))
                control = std::move(file);
    }

    std::optional<SysFs::SysPath> EShopTitle::ValidateAllFiles(const std::vector<SysFs::SysPath>& files) const {
        Security::Checksum checksum;

        std::array<u8, 32> result;
        std::vector<u8> buffer(4 * 1024 * 1024);
        for (const auto& path : files) {
            bool cnmt{};
            auto filename{path};
            if (GetEntryFormat(filename) != ContainedFormat::Nca)
                continue;
            while (filename.has_extension()) {
                if (GetEntryFormat(filename) == ContainedFormat::Cnmt)
                    cnmt = true;
                filename = filename.replace_extension();
            }
            if (cnmt)
                continue;

            auto expected{HexTextToByteArray<16>(filename.string())};
            if (IsEmpty(expected))
                continue;

            bool match{};
            const auto stream{std::make_unique<SysFs::ContinuousBlock>(pfs->OpenFile(path))};
            if (!stream)
                throw Except("The current NCA does not have valid backing");

#if 1
            // Skipping files larger than 4MB for now
            if (stream->GetSize() > buffer.size())
                continue;
#endif

            while (auto remain{stream->RemainBytes()}) {
                if (remain > buffer.size())
                    remain = buffer.size();
                const auto size{stream->Read(buffer.data(), remain)};
                checksum.Update(buffer.data(), size);
            }
            checksum.Finish(std::span(result));
            match = IsEqual(std::span(result).subspan(0, 16), std::span(expected));
            if (!match)
                return path;
        }

        return {};
    }
}
