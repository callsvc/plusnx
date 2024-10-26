#pragma once

#include <types.h>
#include <sys_fs/fs_types.h>
#include <core/context.h>

namespace Plusnx::Loader {
    constexpr auto MinimumAppSize{1024 * 256};
    enum class AppType {
        Invalid,
        Nsp,
        Xci,
        Nro
    };

    enum class SectionType {
        Text,
        Ro,
        Data,
        Bss
    };

    enum class LoaderStatus {
        None,
        InvalidMagicValue
    };

    class AppLoader {
    public:
        virtual ~AppLoader() = default;

        AppLoader(const AppType app, const u32 magic = 0) : type(app), validMagic(magic) {}
        virtual void Load(std::shared_ptr<Core::Context>& context) {}
        virtual SysFs::FileBackingPtr GetMainRomFs() {
            return {};
        }

        AppType type;
        u32 validMagic;
        LoaderStatus status{};

        std::span<u8> text;
        std::span<u8> data;
        std::span<u8> ro;

    protected:
        bool CheckHeader(const SysFs::FileBackingPtr& file);
        void DisplaySection(SectionType type) const;
        static void DisplayRomFsContent(const SysFs::FileBackingPtr& romFs);
    };
}