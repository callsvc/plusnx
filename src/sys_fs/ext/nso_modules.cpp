#include <sys_fs/ext/nso_modules.h>

namespace Plusnx::SysFs::Ext {
    void NsoModules::LoadProgramImage(const u64 startAddr, const std::vector<FileBackingPtr>& files) {
        for (const auto& binary : files) {
            const auto acceptArgs{binary->path == "rtld"};

            auto module{std::make_shared<Nx::NsoCore>(binary)};

            const auto baseAddr{startAddr};
            module->Load(startAddr, acceptArgs);
            modules.insert_or_assign(baseAddr, std::move(module));
        }
    }
}
