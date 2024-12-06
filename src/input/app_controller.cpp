#include <thread>
#include <input/app_controller.h>

namespace Plusnx::Input {
    AppController::AppController(const std::shared_ptr<Core::Context>& context) : process(context->process), kHid(std::make_unique<GenericKernel::Types::KSharedMemory>(*context->kernel)) {
        pthread_setname_np(pthread_self(), "Input Thread");
        hid = reinterpret_cast<HidMemoryLayout*>(kHid->Allocate(nullptr, sizeof(HidMemoryLayout), process));
    }

    AppController::~AppController() {
        if (hid)
            kHid->Free(reinterpret_cast<u8*>(hid), sizeof(HidMemoryLayout), process);
    }

    void AppController::Initialize(std::unique_ptr<FrontendUserControllerDriver>&& controller) {
        joycon = std::move(controller);
        if (!joycon)
            return;

        joycon->PickAJoystick();
    }
}
