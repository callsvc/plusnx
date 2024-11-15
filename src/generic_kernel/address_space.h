#pragma once

#include <types.h>
namespace Plusnx::GenericKernel {
    enum class AddressSpaceType {
        AddressSpace32Bit,
        AddressSpace64BitOld, // < 36 Bits
        AddressSpace32BitNoReserved, // < 32 Bits
        AddressSpace64Bit // < 39 Bits
    };

    u32 GetHostBitsCount(AddressSpaceType type);
}