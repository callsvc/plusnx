#pragma once

#include <SDL2/SDL_audio.h>

#include <types.h>
namespace Plusnx::Audio {
    class OutputDevices {
    public:
        OutputDevices();
        ~OutputDevices();

        void MigrateDriver();

        char* device{};
        bool driver{};
        SDL_AudioDeviceID audio;
    };
}