#include <print>

#include <sys_fs/nx/readonly_filesystem.h>
#include <loader/app_loader.h>
#include <loader/nx_executable.h>
namespace Plusnx::Loader {
    ContainedFormat GetEntryFormat(const SysFs::SysPath& filename) {
        const auto& extension{filename.extension()};
        if (extension == ".nca")
            return ContainedFormat::Nca;
        if (extension == ".tik")
            return ContainedFormat::Ticket;
        if (extension == ".cnmt")
            return ContainedFormat::Cnmt;

        return ContainedFormat::Invalid;
    }

    bool AppLoader::CheckHeader(const SysFs::FileBackingPtr& file) {
        const auto offset = [&] -> u64 {
            if (type == AppType::Nro)
                return offsetof(NroHeader, magic);
            return {};
        }();
        u64 requested{MinimumAppSize};
        if (type == AppType::Nsp)
            requested *= 1'000;

        bool result{};
        if (file->GetSize() < requested) {
            status = LoaderStatus::BrokenFile;
            return result;
        }
        if (result = file->Read<u32>(offset) == validMagic; !result)
            status = LoaderStatus::InvalidMagicValue;

        return result;
    }

    void AppLoader::DisplaySection(const SectionType type) const {
        std::stringstream ss;
        if (type == SectionType::Text)
            throw Except("Text sections are not supported");

        const auto strings = [&] {
            if (type == SectionType::Ro)
                return StringViewBuilder(ro);
            return StringViewBuilder(data);
        }();
        ss << strings;

        std::print("{}\n", ss.str());
    }

    void AppLoader::DisplayRomFsContent(const SysFs::FileBackingPtr& content) {
        const auto backing{std::make_unique<SysFs::Nx::ReadOnlyFilesystem>(content)};
        for (const auto& filename : backing->ListAllFiles()) {
            std::print("File name from this RomFS: {}\n", filename.string());
        }
    }
}
