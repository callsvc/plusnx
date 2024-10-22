#include <loader/cartridge.h>

#include <sys_fs/fsys/regular_file.h>
namespace Plusnx::Loader {
    Cartridge::Cartridge(const SysFs::RegularFilePtr& xci) :
        AppLoader(AppType::Xci) {
        assert(xci->GetSize());
    }
}
