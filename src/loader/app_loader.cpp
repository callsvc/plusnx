#include <print>

#include <sys_fs/nx/readonly_filesystem.h>
#include <loader/nx_executable.h>
#include <loader/app_loader.h>
namespace Plusnx::Loader {
    std::string GetTypeName(const AppType type) {
        switch (type) {
            case AppType::Nca:
                return "NCA";
            case AppType::Nro:
                return "NRO";
            case AppType::Nsp:
                return "NSP";
            case AppType::Xci:
                return "XCI";
            case AppType::Nso:
                return "NSO";
            case AppType::GameFs:
                return "GameFS";
            default:
                throw runtime_exception("Unknown app type");
        }
    }

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

    void AppLoader::DisplaySection(const SectionType type) const {
        std::stringstream ss;
        if (type == SectionType::Text)
            throw runtime_exception("Text sections are not supported");

        ss << StringViewBuilder(GetExeSection(type));
        std::print("{}\n", ss.str());
    }
}
