#pragma once

#include <vector>
#include <SDL2/SDL_audio.h>
#include <types.h>
namespace Plusnx::Audio {
    constexpr auto SampleRate{48'000}; // 48kHz
    constexpr auto Channels{2};
    constexpr auto Format{AUDIO_S16SYS};

    class Device {
    public:
        Device(i32 device);

        virtual ~Device();
        void Initialize(const char* hardware, SDL_AudioSpec& specs);

        virtual void Start();
        virtual void Stop();
    protected:
        std::vector<const char*> available;
        const char* target{nullptr};

        i32 recorder{};
        SDL_AudioDeviceID audio{};
    };
}