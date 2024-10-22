#pragma once

#include <types.h>
#include <sys_fs/fs_types.h>
namespace Plusnx::Loader {
    enum class AppType {
        Invalid,
        Nsp,
        Xci
    };

    class AppLoader {
    public:
        AppLoader(const AppType app, const u32 magic = 0) : type(app), upperMagic(magic) {}

        AppType type;
        u32 upperMagic;
    protected:
        bool CheckHeader(const SysFs::FileBackingPtr& file) const;
    };
}