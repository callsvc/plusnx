#pragma once

#include <loader/app_loader.h>
#include <sys_fs/fs_types.h>
namespace Plusnx::Os {
    Loader::AppType GetAppTypeByFilename(const SysFs::SysPath& filename);

    std::shared_ptr<Loader::AppLoader> MakeLoader(const std::shared_ptr<Core::Context>& context, const SysFs::FileBackingPtr& file);
}