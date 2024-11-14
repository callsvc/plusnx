#include <sys_fs/nx/nso_core.h>

namespace Plusnx::SysFs::Nx {
    NsoCore::NsoCore(const FileBackingPtr& nso) {
        if (nso->Read(content) < sizeof(content))
            return;
        assert(content.magic == ConstMagic<u32>("NSO0"));
    }
}
