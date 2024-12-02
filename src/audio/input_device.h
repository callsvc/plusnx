#pragma once

#include <audio/device.h>
namespace Plusnx::Audio {
    class InputDevice final : public Device {
    public:
        InputDevice();

        void HandleAudioEvent(const std::span<u8>& stream) override;
    private:
        SDL_AudioSpec attributes{};
    };
}