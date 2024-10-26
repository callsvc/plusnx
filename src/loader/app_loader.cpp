#include <print>

#include <sys_fs/nx/readonly_filesystem.h>
#include <loader/app_loader.h>
#include <loader/nx_executable.h>
namespace Plusnx::Loader {
    bool AppLoader::CheckHeader(const SysFs::FileBackingPtr& file) {
        const auto offset = [&] -> u64 {
            if (type == AppType::Nro)
                return offsetof(NroHeader, magic);
            return {};
        }();
        const auto result = [&] -> bool {
            u64 requested{MinimumAppSize};
            if (type == AppType::Nsp)
                requested *= 1'000;

            if (file->GetSize() < requested) {
                return {};
            }
            return file->Read<u32>(offset) == validMagic;
        }();
        if (!result)
            status = LoaderStatus::InvalidMagicValue;
        return result;
    }

    void AppLoader::DisplaySection(const SectionType type) const {
        std::stringstream ss;
        if (type == SectionType::Text)
            throw std::runtime_error("Text sections are not supported");

        const auto strings = [&] {
            if (type == SectionType::Ro)
                return StringViewBuilder(ro);
            return StringViewBuilder(data);
        }();
        ss << strings;

        std::print("{}\n", ss.str());
    }

    void AppLoader::DisplayRomFsContent(const SysFs::FileBackingPtr& romFs) {
        const SysFs::Nx::ReadOnlyFilesystem readOnly(romFs);
        for (const auto& filename : readOnly.ListAllFiles()) {
            std::print("File name from this RomFS: {}\n", filename.string());
        }
    }
}
