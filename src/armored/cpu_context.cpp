#include <sys_fs/fsys/regular_file.h>
#include <armored/cpu_context.h>
namespace Plusnx::Armored {
    CpuContext::CpuContext() : owner(std::this_thread::get_id()) {
        {
            SysFs::FSys::RegularFile entropy("/dev/urandom");
            entropy.Read(identifier);
        }

        std::memset(&ctx, 0, sizeof(ctx));
        std::memset(&host, 0, sizeof(host));
    }
}
