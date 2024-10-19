#include <gsk/types/kprocess.h>

namespace Plusnx::Gsk::Types {
    void KProcess::Initialize() {
        pid = kernel.CreateProcessId();
    }
    void KProcess::Destroy() {
        pid = {};
    }
}
