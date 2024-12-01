#include <audio/input_device.h>

namespace Plusnx::Audio {
    constexpr auto identifier{1};
    InputDevice::InputDevice(): Device(identifier) {
        Initialize(nullptr, attributes);
        SDL_ClearQueuedAudio(audio);
        Device::Stop();
    }

    void InputDevice::HandleAudioEvent([[maybe_unused]] const std::span<u8>& stream) {
    }
}
