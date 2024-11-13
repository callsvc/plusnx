#pragma once

#include <core/context.h>
#include <sys_fs/fs_types.h>

namespace Plusnx::SysFs::Nx {
    class ReadOnlyFilesystem;
    class PartitionFilesystem;
}
namespace Plusnx::Loader {
    enum class MinimumAppSize {
        Nro = 1024 * 200,
        Nsp = 1024 * Nro
    };

    enum class ContainedFormat {
        Invalid,
        Nca,
        Ticket,
        Cnmt,
        Xml
    };
    ContainedFormat GetEntryFormat(const SysFs::SysPath& filename);

    enum class AppType {
        Invalid,
        Nsp,
        Xci,
        Nro,
        GameFs
    };

    enum class SectionType {
        Text,
        Ro,
        Data,
        Bss
    };

    enum class LoaderStatus {
        None,
        InvalidMagicValue,
        BrokenFile
    };

    class AppLoader {
    public:
        virtual ~AppLoader() = default;

        AppLoader(const AppType app, const u32 magic = 0) : type(app), validMagic(magic) {}
        virtual bool ExtractFilesInto([[maybe_unused]] const SysFs::SysPath& path) const {
            return {};
        }
        virtual void Load(std::shared_ptr<Core::Context>& context) = 0;

        AppType type;
        u32 validMagic;
        LoaderStatus status{LoaderStatus::None};

        std::shared_ptr<SysFs::Nx::ReadOnlyFilesystem> romfs;
        std::shared_ptr<SysFs::Nx::PartitionFilesystem> exefs;
        std::shared_ptr<SysFs::Nx::PartitionFilesystem> icon;
        std::shared_ptr<SysFs::FileBacking> control;

    protected:
        bool CheckHeader(const SysFs::FileBackingPtr& file);
        static void DisplayRomFsContent(const std::shared_ptr<SysFs::Nx::ReadOnlyFilesystem>& content);
    };

    class ExecutableAppLoader : public AppLoader {
    public:
        ExecutableAppLoader(const AppType app, const u32 magic = 0) : AppLoader(app, magic) {}

        virtual std::span<u8> GetExeSection(SectionType type) const = 0;
        void DisplaySection(SectionType type) const;
    };
}