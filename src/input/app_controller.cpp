#include <thread>
#include <input/app_controller.h>

namespace Plusnx::Input {
    AppController::AppController(const std::shared_ptr<Core::Context>& context) :
        hidBridge(std::make_shared<SharedHidBridge>(context)) {

        pthread_setname_np(pthread_self(), "Input Thread");
    }
    void AppController::Initialize(std::unique_ptr<FrontendUserControllerDriver>&& controller) {
        joycon = std::move(controller);
        if (!joycon)
            return;

        joycon->PickAJoystick();
    }
}
