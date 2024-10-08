#include <context.h>

namespace Plusnx {
    Context::Context() :
        provider(std::make_shared<SysFs::Provider>()),
        gpu(std::make_shared<Graphics::GPU>()) {

        rootDir = provider->GetRoot();
    }

    std::filesystem::path Context::GetSystemPath(const SystemPaths tagged) {
        switch (tagged) {
            case BaseDirectoryFs:
                return rootDir;
            default:
                assert(0);
        }
    }
}
