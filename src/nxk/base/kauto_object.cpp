#include <nxk/base/kauto_object.h>

namespace Plusnx::Nxk::Base {
    KAutoObject::KAutoObject(Kernel& generic, const KAutoType type): KBaseType(generic, type) {
    }

    void KAutoObject::IncrementReference() {
        std::lock_guard lock(refLock);
        assert(refs);
        refs++;
    }
    void KAutoObject::DecrementReference() {
        std::lock_guard lock(refLock);

        if (!refs)
            assert(0);

        refs--;
    }
}
