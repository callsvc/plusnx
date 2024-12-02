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

    enum class AppType {
        Invalid,
        Nsp,
        Xci,
        Nca,
        Nro,
        Nso,
        GameFs
    };
    std::string GetTypeName(AppType type);

    enum class SectionType {
        Text,
        Ro,
        Data,
        Bss,
        Invalid
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
        virtual void Load(std::shared_ptr<Core::Context>& context) = 0;

        virtual bool ExtractFilesInto([[maybe_unused]] const SysFs::SysPath& path) const { return {}; }
        virtual std::shared_ptr<SysFs::Nx::ReadOnlyFilesystem> GetRomFs([[maybe_unused]] bool isControl) const { return {}; }
        virtual SysFs::FileBackingPtr GetNpdm() const { return {}; }

        AppType type{};
        u64 titleId{};
        u32 validMagic{};
        LoaderStatus status{LoaderStatus::None};

    protected:
        bool CheckHeader(const SysFs::FileBackingPtr& file);
        static void DisplayRomFsContent(const std::shared_ptr<SysFs::Nx::ReadOnlyFilesystem>& content);

        virtual std::span<u8> GetExeSection([[maybe_unused]] SectionType type) const { return {}; }
        void DisplaySection(SectionType type) const;
    };
}