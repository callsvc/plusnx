#include <ranges>
#include <sys_fs/bounded.h>

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
                DisplayRomFsContent(romfs.value());
        }
        assert(assets + content.size == nro->GetSize());

        text = ReadSectionContent(nro, SectionType::Text, offset);
        ro = ReadSectionContent(nro, SectionType::Ro, offset + text.size());
        data = ReadSectionContent(nro, SectionType::Data, offset + text.size() + ro.size());
    }

    void NxExecutable::ReadAssets(const SysFs::FileBackingPtr& nro) {
        assert(!assetHeader.version);

        for (const auto& [index, section] : std::ranges::views::enumerate(assetHeader.assets)) {
            if (!section.size)
                continue;

            auto assetFile = [&] {
                const auto offset{content.size + section.offset};
                if (index == 0)
                    assert(section.offset == sizeof(AssetHeader));
                return std::make_shared<SysFs::FileBounded>(nro, "", offset, section.size);
            }();
            const auto content = [&] {
                if (!index)
                    return &icon;
                if (index == 1)
                    return &control;
                return &romfs;
            }();
            content->emplace(assetFile);
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
            throw std::runtime_error("Invalid section type");
        }();

        const std::span result(&program[offset], size);
        if (nro->Read(result.data(), result.size(), fileOffset) != size)
            throw std::runtime_error("Cannot read section");

        return result;
    }

    void NxExecutable::Load(std::shared_ptr<Core::Context>& context) {
        [[maybe_unused]] auto& process{context->process};
    }
}
