#pragma once

#include <SDL2/SDL_audio.h>

#include <types.h>
#include <audio/device.h>
namespace Plusnx::Audio {
    class OutputDevice final : public Device {
    public:
        OutputDevice();

        void Start() override;
    };
}
