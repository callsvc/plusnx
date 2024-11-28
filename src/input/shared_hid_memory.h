#pragma once

#include <core/context.h>
#include <generic_kernel/types/kshared_memory.h>
#include <input/controller_types.h>
namespace Plusnx::Input {

    struct alignas(0x40000) SharedHidMemory {
    };

    static_assert(sizeof(SharedHidMemory) == 0x40000);

    class SharedHidBridge {
    public:
        SharedHidBridge(const std::shared_ptr<Core::Context>& context);

    private:
        std::unique_ptr<GenericKernel::Types::KSharedMemory> shrHid;
    };
}