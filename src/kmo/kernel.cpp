#include <kmo/kernel.h>
namespace Plusnx::Kmo {
    u64 Kernel::CreateProcessId() {
        return pitch++;
    }

}