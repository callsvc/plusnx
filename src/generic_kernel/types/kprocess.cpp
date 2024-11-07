#include <generic_kernel/types/kprocess.h>

namespace Plusnx::GenericKernel::Types {
    KProcess::KProcess(Kernel& kernel) : KBaseType(kernel, KType::KProcess), us(kernel.us) {
    }

    void KProcess::Initialize() {
        pid = kernel.CreateProcessId();
    }
    void KProcess::Destroy() {
        pid = {};
    }
}
