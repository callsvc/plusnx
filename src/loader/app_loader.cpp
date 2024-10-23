#include <loader/app_loader.h>

namespace Plusnx::Loader {
    bool AppLoader::CheckHeader(const SysFs::FileBackingPtr& file) const {
        const auto magic{file->Read<u32>()};
        const auto result = [&] -> bool {
            if (file->GetSize() < MinimumAppSize) {
                return {};
            }
            return magic == upperMagic;
        }();
        return result;
    }
}
