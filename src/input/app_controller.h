#pragma once

#include <input/shared_hid_memory.h>
#include <input/controller_types.h>
namespace Plusnx::Input {

    class AppController {
    public:
        AppController(const std::shared_ptr<Core::Context>& context);
        void Initialize(std::unique_ptr<FrontendUserControllerDriver>&& controller);

        std::shared_ptr<SharedHidBridge> hidBridge;
        std::unique_ptr<FrontendUserControllerDriver> joycon;
    };
}
