#pragma once

#include <SDL2/SDL_audio.h>

#include <types.h>
#include <audio/device.h>
namespace Plusnx::Audio {
    class OutputDevice final : public Device {
    public:
        OutputDevice();

        void HandleAudioEvent(const std::span<u8>& stream) override;
        void Start() override;
    };
}
