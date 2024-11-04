#pragma once

#include <types.h>
#include <sys_fs/fs_types.h>
#include <core/context.h>

namespace Plusnx::Loader {
    constexpr auto MinimumAppSize{1024 * 200};

    enum class ContainedFormat {
        Invalid,
        Nca,
        Ticket,
        Cnmt
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
        virtual void Load(std::shared_ptr<Core::Context>& context) {}

        AppType type;
        u32 validMagic;
        LoaderStatus status{LoaderStatus::None};

        std::optional<SysFs::FileBackingPtr> icon;
        std::optional<SysFs::FileBackingPtr> control;
        std::optional<SysFs::FileBackingPtr> romfs;

        std::span<u8> text;
        std::span<u8> data;
        std::span<u8> ro;

    protected:
        bool CheckHeader(const SysFs::FileBackingPtr& file);
        void DisplaySection(SectionType type) const;
        static void DisplayRomFsContent(const SysFs::FileBackingPtr& content);
    };
}