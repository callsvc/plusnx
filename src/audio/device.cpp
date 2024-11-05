#include <print>

#include <audio/device.h>
namespace Plusnx::Audio {
    Device::Device(const i32 device) : recorder(device) {
        const auto devices{SDL_GetNumAudioDevices(device)};

        for (i32 out{}; out < devices; out++) {
            std::print("Available sound {} device: {}\n", recorder ? "input" : "output", SDL_GetAudioDeviceName(out, recorder));
            available.push_back(SDL_GetAudioDeviceName(out, recorder));
        }
    }

    Device::~Device() {
        if (target)
            free(const_cast<char*>(target));
        SDL_CloseAudioDevice(audio);
    }

    void Device::Initialize(const char* hardware, SDL_AudioSpec& specs) {
        SDL_AudioSpec current;
        char* sound;
        SDL_GetDefaultAudioInfo(&sound, &current, recorder);
        if (hardware) {
            assert(ContainsValue(available, hardware));
            target = strdup(hardware);
        } else {
            target = strdup(sound);
            specs = current;
        }

        SDL_free(sound);
        audio = SDL_OpenAudioDevice(target, recorder, &specs, &current, 0);
        specs = current;
    }

    void Device::Start() {
        SDL_PauseAudioDevice(audio, 0);
    }
    void Device::Stop() {
        SDL_PauseAudioDevice(audio, 1);
    }
}
