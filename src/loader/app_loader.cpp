#include <print>

#include <sys_fs/nx/readonly_filesystem.h>
#include <loader/nx_executable.h>
#include <loader/app_loader.h>
namespace Plusnx::Loader {
    bool AppLoader::CheckHeader(const SysFs::FileBackingPtr& file) {
        const auto offset = [&] -> u64 {
            if (type == AppType::Nro)
                return offsetof(NroHeader, magic);
            return {};
        }();
        const auto requested = [&] -> u64 {
            if (type == AppType::Nro)
                return static_cast<u64>(MinimumAppSize::Nro);
            return static_cast<u64>(MinimumAppSize::Nsp);
        }();

        if (file->GetSize() < requested) {
            status = LoaderStatus::BrokenFile;
            return {};
        }
        bool result{};
        if (result = file->Read<u32>(offset) == validMagic; !result)
            status = LoaderStatus::InvalidMagicValue;
        return result;
    }
    void AppLoader::DisplayRomFsContent(const std::shared_ptr<SysFs::Nx::ReadOnlyFilesystem>& content) {
        for (const auto& filename : content->ListAllFiles()) {
            std::print("File name from this RomFS: {}\n", filename.string());
        }
    }

    void ExecutableAppLoader::DisplaySection(const SectionType type) const {
        std::stringstream ss;
        if (type == SectionType::Text)
            throw runtime_plusnx_except("Text sections are not supported");

        ss << StringViewBuilder(GetExeSection(type));
        std::print("{}\n", ss.str());
    }
}
