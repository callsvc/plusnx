#pragma once

#include <SDL2/SDL_audio.h>

#include <audio/input_device.h>
#include <audio/output_device.h>
#include <types.h>
namespace Plusnx::Audio {
    class Sound {
    public:
        Sound();
        ~Sound();

        std::unique_ptr<OutputDevice> output;
        std::unique_ptr<InputDevice> input;
    private:
        bool driver;
        void MigrateDriver();
    };
}