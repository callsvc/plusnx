#pragma once

#include <types.h>

#include <generic_kernel/constants.h>
#include <generic_kernel/memory_nx.h>
namespace Plusnx::GenericKernel {
    namespace Types {
        class KProcess;
    }
    enum class AddressSpaceType {
        AddressSpace32Bit,
        AddressSpace64BitOld, // < 36 Bits
        AddressSpace32BitNoReserved, // < 32 Bits
        AddressSpace64Bit // < 39 Bits
    };

    u32 GetHostBitsCount(AddressSpaceType type);
    void CreateUserAddressSpace(const std::shared_ptr<Types::KProcess>& process, const std::unique_ptr<MemoryNx>& buffer, const std::array<RegionProperties*, 0x5>& regions);
}
