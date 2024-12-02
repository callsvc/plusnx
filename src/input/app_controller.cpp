#include <thread>
#include <input/app_controller.h>

namespace Plusnx::Input {
    AppController::AppController(const std::shared_ptr<Core::Context>& context) : kHid(std::make_unique<GenericKernel::Types::KSharedMemory>(*context->kernel)) {
        pthread_setname_np(pthread_self(), "Input Thread");
    }
    void AppController::Initialize(std::unique_ptr<FrontendUserControllerDriver>&& controller) {
        joycon = std::move(controller);
        if (!joycon)
            return;

        joycon->PickAJoystick();
    }
}
