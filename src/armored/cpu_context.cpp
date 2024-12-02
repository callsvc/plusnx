#include <sys_fs/fsys/regular_file.h>
#include <armored/cpu_context.h>
namespace Plusnx::Armored {
    CpuContext::CpuContext() {
        {
            SysFs::FSys::RegularFile entropy("/dev/urandom");
            entropy.Read(ccid);
        }

        std::memset(&ctx, 0, sizeof(ctx));
        std::memset(&hrc, 0, sizeof(hrc));
    }
}
