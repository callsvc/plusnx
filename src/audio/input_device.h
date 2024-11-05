#pragma once

#include <audio/device.h>
namespace Plusnx::Audio {
    class InputDevice final : public Device {
    public:
        InputDevice();

    private:
        SDL_AudioSpec attributes{};
    };
}