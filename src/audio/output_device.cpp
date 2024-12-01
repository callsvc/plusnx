#include <print>
#include <random>
#include <limits>

#include <SDL2/SDL.h>
#include <audio/output_device.h>
namespace Plusnx::Audio {
    constexpr auto identifier{0};
    OutputDevice::OutputDevice(): Device(identifier) {

        SDL_AudioSpec required{};
        required.freq = SampleRate;
        required.format = Format;
        required.channels = Channels;
        const auto name{SDL_GetAudioDeviceName(0, identifier)};
        Initialize(name, required);
    }

    void OutputDevice::HandleAudioEvent(const std::span<u8>& stream) {
        std::random_device rd;
        std::mt19937 engine{rd()};
        std::uniform_int_distribution<i16> distribution{0, 1 << 8};

        std::span samples{reinterpret_cast<i16*>(stream.data()), stream.size() / 2};
        for (u64 smp{}; smp < samples.size(); smp++) {
            const auto value{distribution(engine)};
            const auto signal{distribution(engine) % 2 * 2 - 1};

            samples[smp] = value * signal;
        }
    }

    void OutputDevice::Start() {
        std::print("Let's play some noise to test the sound output\n");
        Device::Start();
    }
}
