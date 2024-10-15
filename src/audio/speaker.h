#pragma once

#include <SDL2/SDL_audio.h>

#include <types.h>
namespace Plusnx::Audio {
    class Speaker {
    public:
        Speaker();
        ~Speaker();

        void MigrateDriver();

        char* device{};
        bool driver{};
        SDL_AudioDeviceID audio;
    };
}