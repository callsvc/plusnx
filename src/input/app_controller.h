#pragma once

#include <generic_kernel/types/kshared_memory.h>
#include <input/controller_types.h>

#include <core/context.h>
namespace Plusnx::Input {

    class AppController {
    public:
        AppController(const std::shared_ptr<Core::Context>& context);
        ~AppController();
        void Initialize(std::unique_ptr<FrontendUserControllerDriver>&& controller);

        std::unique_ptr<FrontendUserControllerDriver> joycon;
        std::shared_ptr<GenericKernel::Types::KProcess> process;
    private:
        std::unique_ptr<GenericKernel::Types::KSharedMemory> kHid;
        HidMemoryLayout* hid{nullptr};
    };
}
