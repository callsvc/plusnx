#include <generic_kernel/address_space.h>

#if !defined(NATIVE_CODE_EXECUTION)
#define NATIVE_CODE_EXECUTION 0
#endif

namespace Plusnx::GenericKernel {
    u32 GetHostBitsCount(AddressSpaceType type) {
#if NATIVE_CODE_EXECUTION
        if (type == AddressSpaceType::Guest64Bit)
            return 38;
#else
        if (type == AddressSpaceType::Guest64Bit)
            return 39;
#endif
        if (type == AddressSpaceType::Old64Bit)
            return 36;

        return 32;
    }
}