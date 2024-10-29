#pragma once

#include <generic_kernel/address_space.h>
namespace Plusnx::GenericKernel {
    class UserSpace {
    public:
        UserSpace() = default;

        AddressSpaceType type;
        void CreateProcessMemory(AddressSpaceType type);
    };
}