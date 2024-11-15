#include <loader/main_executable.h>

namespace Plusnx::Loader {
    MainExecutable::MainExecutable(const SysFs::FileBackingPtr &nso) :
        ExecutableAppLoader(AppType::Nso), nso(std::make_unique<SysFs::Nx::NsoCore>(nso)) {
    }
    void MainExecutable::Load(std::shared_ptr<Core::Context>& context) {}
}
