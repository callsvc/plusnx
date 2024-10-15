#include <context.h>

namespace Plusnx {
    Context::Context() :
        provider(std::make_shared<SysFs::Provider>()),
        gpu(std::make_shared<Video::GPU>()),
        speaker(std::make_shared<Audio::Speaker>()) {
    }

    SysFs::SysPath Context::GetSystemPath(const SystemPaths tagged) const {
        switch (tagged) {
            case BaseDirectoryFs:
                return provider->GetRoot();
            default:
                assert(0);
        }
    }
}
