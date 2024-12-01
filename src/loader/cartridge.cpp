#include <ranges>

#include <sys_fs/nx/partition_filesystem.h>
#include <sys_fs/nx/readonly_filesystem.h>
#include <sys_fs/layered_fs.h>
#include <sys_fs/nx/content_metadata.h>

#include <loader/cartridge.h>
namespace Plusnx::Loader {
    std::string GetMemoryCardSize(const RomSize size) {
        switch (size) {
            case RomSize::MinorCard0:
                return "1GB";
            case RomSize::MinorCard1:
                return "2GB";
            case RomSize::MediumCard0:
                return "4GB";
            case RomSize::MediumCard1:
                return "8GB";
            case RomSize::BiggerCard0:
                return "16GB";
            case RomSize::BiggerCard1:
                return "32GB";

            default:
                throw runtime_exception("Unknown memory size");
        }
    }

    Cartridge::Cartridge(const std::shared_ptr<Security::Keyring>& _keys, const SysFs::FileBackingPtr& xci) : AppLoader(AppType::Xci), keys(_keys) {

        assert(xci->GetSize() > sizeof(content));
        if (xci->Read(content) != sizeof(content))
            return;

        const auto& header{content.cardHeader};
        assert(header.magic == ConstMagic<u32>("HEAD"));
        assert(header.backupArea == 0xFFFFFFFF);

        std::print("Memory card capacity: {}\n", GetMemoryCardSize(header.size));
        const auto pfsSize{xci->GetSize() - header.pfsBeginAddr + header.pfsSize};
        if (auto partition{std::make_unique<SysFs::FileLayered>(xci, xci->path, header.pfsBeginAddr, pfsSize)})
            pfs = std::make_unique<SysFs::Nx::PartitionFilesystem>(std::move(partition));

        if (!pfs)
            return;

        auto files{pfs->ListAllFiles()};
        // Post version 4.0.0, the content of this section has been moved to 'logo'
        if (ContainsValue(files, "normal")) {
            files.erase(std::ranges::find(files, "normal"));
        }

        for (const auto& partition : files) {
            assert(pfs->Exists(partition));
            if (const auto subpart{std::make_unique<SysFs::Nx::PartitionFilesystem>(pfs->OpenFile(partition))})
                ListAllNca(std::move(subpart));
        }
    }

    void Cartridge::Load([[maybe_unused]] std::shared_ptr<Core::Context>& context) {
    }

    SysFs::FileBackingPtr Cartridge::GetNpdm() const {
        if (exefs)
            return exefs->OpenFile("main.npdm");
        return {};
    }

    std::shared_ptr<SysFs::Nx::ReadOnlyFilesystem>
    Cartridge::GetRomFs(const bool isControl) const {
        if (isControl)
            return control;
        return romfs;
    }

    void Cartridge::ListAllNca(const std::unique_ptr<SysFs::Nx::PartitionFilesystem>& partition) {
        auto files{partition->ListAllFiles()};
        if (files.empty())
            return;

        std::vector<SysFs::Nx::ContentMetadata> cnmts;
        for (const auto& file : files) {
            // We are not currently indexing the NCAs correctly
            if (exefs && logo && romfs && control)
                break;

            const auto nca{std::make_unique<SysFs::Nx::NcaCore>(keys, partition->OpenFile(file))};

            if (nca->type == SysFs::Nx::ContentType::PublicData ||
                nca->type == SysFs::Nx::ContentType::Manual)
                continue;

            auto ncaFiles{nca->GetBackingFiles()};
            std::shared_ptr<SysFs::Nx::PartitionFilesystem> firstPfs{};
            std::shared_ptr<SysFs::Nx::PartitionFilesystem> secondPfs{};

            std::shared_ptr<SysFs::Nx::ReadOnlyFilesystem> ncaRomfs{};

            for (const auto& [type, ncaFile] : ncaFiles) {
                switch (type) {
                    case SysFs::Nx::FsType::PartitionFs:
                        if (!firstPfs)
                            firstPfs = std::make_shared<SysFs::Nx::PartitionFilesystem>(ncaFile);
                        else if (!secondPfs)
                            secondPfs = std::make_shared<SysFs::Nx::PartitionFilesystem>(ncaFile);
                        else
                            assert(false);
                        break;
                    case SysFs::Nx::FsType::RomFs:
                        ncaRomfs = std::make_shared<SysFs::Nx::ReadOnlyFilesystem>(ncaFile);
                    default: {}
                }
            }
            if (firstPfs)
                assert(firstPfs->first);

            if (file.stem().extension() == ".cnmt" && nca->type == SysFs::Nx::ContentType::Meta) {
                cnmts.emplace_back(firstPfs->OpenFile(*firstPfs->first));
                if (!titleId)
                    titleId = cnmts.back().programId;
            }

            auto&& GetValidPfs = [&] {
                if (firstPfs)
                    return firstPfs;
                return secondPfs;
            };

            if (nca->type == SysFs::Nx::ContentType::Program) {
                if (!exefs && firstPfs && IsAExeFsPartition(firstPfs))
                    exefs = std::move(firstPfs);
                else if (!exefs && secondPfs && IsAExeFsPartition(secondPfs))
                    exefs = std::move(secondPfs);

                if (!logo)
                    logo = GetValidPfs();

                if (!romfs && ncaRomfs)
                    romfs = std::move(ncaRomfs);
            }
            if (nca->type == SysFs::Nx::ContentType::Control)
                if (!control && ncaRomfs)
                    control = std::move(ncaRomfs);
        }

        for (const auto& [index, metas] : std::ranges::views::enumerate(cnmts)) {
            if (metas.programId != titleId)
                std::print("Metadata of index {}, appears to have an unknown or altered program ID, correct value {}, found value {}\n", index, titleId, metas.programId);
        }
    }
}
