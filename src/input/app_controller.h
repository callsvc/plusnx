#pragma once

#include <generic_kernel/types/kshared_memory.h>
#include <input/controller_types.h>

#include <core/context.h>
namespace Plusnx::Input {

    class AppController {
    public:
        AppController(const std::shared_ptr<Core::Context>& context);
        void Initialize(std::unique_ptr<FrontendUserControllerDriver>&& controller);

        std::unique_ptr<FrontendUserControllerDriver> joycon;

    private:
        std::unique_ptr<GenericKernel::Types::KSharedMemory> kHid;
    };
}
