#include <ranges>
#include <sys_fs/layered_fs.h>
#include <sys_fs/nx/readonly_filesystem.h>
#include <sys_fs/nx/partition_filesystem.h>
#include <sys_fs/extra/insertable_romfs.h>

#include <loader/nx_executable.h>
namespace Plusnx::Loader {
    NxExecutable::NxExecutable(const SysFs::FileBackingPtr& nro) :
        AppLoader(AppType::Nro, ConstMagic<u32>("NRO0")) {
        u64 offset{},
            assets{};
        if (offset = nro->Read(content); offset != sizeof(content))
            return;
        if (!CheckHeader(nro) || status != LoaderStatus::None)
            return;

        assert(nro->GetSize() > content.size);
        program.resize(content.size);

        nro->Read(assetHeader, content.size);
        if (assetHeader.magic == ConstMagic<u32>("ASET")) {
            ReadAssets(nro);
            for (const auto& [_, size] : assetHeader.assets) {
                if (size > 0)
                    assets += size;
            }
            assets += sizeof(AssetHeader);

            if (romfs)
                DisplayRomFsContent(romfs);
        }
        assert(assets + content.size == nro->GetSize());

        textSection = ReadSectionContent(nro, SectionType::Text, offset);
        roSection = ReadSectionContent(nro, SectionType::Ro, offset + textSection.size());
        dataSection = ReadSectionContent(nro, SectionType::Data, offset + textSection.size() + roSection.size());
    }

    void NxExecutable::ReadAssets(const SysFs::FileBackingPtr& nro) {
        assert(!assetHeader.version);

        for (const auto& [index, section] : std::ranges::views::enumerate(assetHeader.assets)) {
            if (!section.size)
                continue;

            auto asset = [&] {
                const auto offset{content.size + section.offset};
                if (!index)
                    assert(section.offset == sizeof(AssetHeader));
                return std::make_shared<SysFs::FileLayered>(nro, !index ? "icon" : index == 1 ? "/control.nacp" : "romfs", offset, section.size);
            }();

            if (index == 2)
                romfs = std::make_shared<SysFs::Nx::ReadOnlyFilesystem>(std::move(asset));
            else if (index == 1)
                nacp = std::move(asset);
            else if (!index)
                icon = std::make_shared<SysFs::Nx::PartitionFilesystem>(std::move(asset));
        }
    }

    std::span<u8> NxExecutable::ReadSectionContent(const SysFs::FileBackingPtr& nro, const SectionType type, const u32 fileOffset) {
        const auto [offset, size] = [&] {
            if (type == SectionType::Text)
                return content.text;
            if (type == SectionType::Ro)
                return content.ro;
            if (type == SectionType::Data)
                return content.data;
            throw exception("Invalid section type");
        }();

        const std::span result(&program[offset], size);
        if (nro->Read(result.data(), result.size(), fileOffset) != size)
            throw exception("Cannot read section");

        return result;
    }

    std::optional<ProcessLoadResult> NxExecutable::Load(std::shared_ptr<Core::Context>& context) {
        [[maybe_unused]] auto& process{context->process};
        return {};
    }
    std::shared_ptr<SysFs::Nx::ReadOnlyFilesystem> NxExecutable::GetRomFs(const bool isControl) const {
        if (!isControl)
            return romfs;
        const auto control{std::make_shared<SysFs::Extra::InsertableRomFs>()};
        control->AddFile(nacp);

        return control;
    }

    std::span<u8> NxExecutable::GetExeSection(const SectionType type) const {
        if (type == SectionType::Text)
            return textSection;
        if (type == SectionType::Ro)
            return roSection;
        if (type == SectionType::Data)
            return dataSection;

        throw exception("The BSS section is invalid for this type of executable");
    }
}
