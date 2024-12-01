#include <generic_kernel/base/kauto_object.h>

namespace Plusnx::GenericKernel::Base {
    KAutoObject::KAutoObject(Kernel& generic, const KAutoType type): KBaseType(generic, type) {
    }

    void KAutoObject::IncrementReference() {
        std::lock_guard lock(refLock);
        assert(refs);
        refs++;
    }
    void KAutoObject::DecrementReference() {
        std::lock_guard lock(refLock);
        assert(refs);

        if (!refs) {}
    }
}
