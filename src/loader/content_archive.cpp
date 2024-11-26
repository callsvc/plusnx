#include <loader/content_archive.h>

namespace Plusnx::Loader {
    ContentArchive::ContentArchive(const std::shared_ptr<Security::Keyring>& keys, const SysFs::FileBackingPtr& nca) :
        AppLoader(AppType::Nca),
        nca(std::make_unique<SysFs::Nx::NcaCore>(keys, nca)) {
    }

    void ContentArchive::Load(std::shared_ptr<Core::Context>& context) {
        const auto files{nca->GetBackingFiles()};

        if (nca->type != SysFs::Nx::ContentType::Program) {
            return;
        }
        for ([[maybe_unused]] const auto& [type, file] : files) {
        }
    }
}
