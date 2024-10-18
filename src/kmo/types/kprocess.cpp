#include <kmo/types/kprocess.h>

namespace Plusnx::Kmo::Types {
    void KProcess::Initialize() {
        pid = kernel.CreateProcessId();
    }
    void KProcess::Destroy() {
        pid = {};
    }
}
