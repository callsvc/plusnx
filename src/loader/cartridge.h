#pragma once

#include <loader/app_loader.h>
namespace Plusnx::Loader {
    class Cartridge final : public AppLoader {
    public:
        Cartridge(const SysFs::RegularFilePtr& xci);
    };
}
