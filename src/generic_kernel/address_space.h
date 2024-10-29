#pragma once

#include <types.h>
namespace Plusnx::GenericKernel {
    enum class AddressSpaceType {
        Real32Bit,
        Old64Bit, // < 36 Bits
        NoReserved32Bit, // < 32 Bits
        Guest64Bit // < 39 Bits
    };

    u32 GetHostBitsCount(AddressSpaceType type);
}