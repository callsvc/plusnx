#include <print>

#include <SDL2/SDL.h>
#include <audio/output_device.h>
namespace Plusnx::Audio {
    constexpr auto identifier{0};
    OutputDevice::OutputDevice(): Device(identifier) {

        SDL_AudioSpec required{.freq = SampleRate, .format = Format, .channels = Channels};
        const auto name{SDL_GetAudioDeviceName(0, identifier)};
        Initialize(name, required);
        Device::Stop();
    }

    void OutputDevice::HandleAudioEvent(const std::span<u8>& stream) {
    }

    void OutputDevice::Start() {
        Device::Start();
        std::print("Let's play some noise to test the sound output\n");
    }
}
