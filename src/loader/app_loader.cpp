#include <loader/app_loader.h>

namespace Plusnx::Loader {
    bool AppLoader::CheckHeader(const SysFs::FileBackingPtr& file) const {
        auto magic{upperMagic};
        file->Read(magic);
        const auto result = [&] -> bool {
            if (file->GetSize() < 1024 * 1024 * 1) {
                return {};
            }
            return magic == upperMagic;
        }();
        return result;
    }
}
