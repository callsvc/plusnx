#include <loader/main_executable.h>

namespace Plusnx::Loader {
    MainExecutable::MainExecutable(const SysFs::FileBackingPtr &nso) :
        ExecutableAppLoader(AppType::Nso), nso(std::make_unique<SysFs::Nx::NsoCore>(nso)) {
    }
    void MainExecutable::Load(std::shared_ptr<Core::Context> &context) {}
    std::shared_ptr<SysFs::Nx::ReadOnlyFilesystem> MainExecutable::GetRomFs(bool isControl) const {
        return nullptr;
    }

    std::span<u8> MainExecutable::GetExeSection(SectionType type) const {
        return {};
    }
}
