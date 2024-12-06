#pragma once

#include <generic_kernel/address_space.h>
#include <types.h>
namespace Plusnx::GenericKernel::Svc {

    enum class ProcessCategory : u32 {
        RegularTitle,
        Kernel
    };

    // https://switchbrew.org/wiki/SVC#CreateProcessParameter
    struct CreateProcessParameter {
        std::array<char, 12> name;
        ProcessCategory category;
        u64 titleId;
        u64 codeAddr;
        u32 codeNumPages;
        union {
            struct {
                u32 is64BitInstruction : 1;
                AddressSpaceType addressType : 3;
                u32 enableDebug : 1;
                u32 enableAslr : 1;
                u32 isApplication : 1;
                u32 pad0 : 5;
            };
        };
        u32 resourceLimitHandle; // ResourceLimitHandle (can be zero)
        u32 systemResourceNumPages;
    };

    static_assert(sizeof(CreateProcessParameter) == 0x30);
}