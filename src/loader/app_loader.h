#pragma once

#include <types.h>
#include <sys_fs/fs_types.h>
#include <core/context.h>

namespace Plusnx::Loader {
    constexpr auto MinimumAppSize{1024 * 1024 * 1};
    enum class AppType {
        Invalid,
        Nsp,
        Xci
    };

    class AppLoader {
    public:
        virtual ~AppLoader() = default;

        AppLoader(const AppType app, const u32 magic = 0) : type(app), upperMagic(magic) {}
        virtual void Load(std::shared_ptr<Core::Context>& process) {}

        AppType type;
        u32 upperMagic;
    protected:
        bool CheckHeader(const SysFs::FileBackingPtr& file) const;
    };
}