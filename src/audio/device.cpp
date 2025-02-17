#include <print>

#include <audio/device.h>
namespace Plusnx::Audio {
    static void RenderSound(void* device, u8* stream, const i32 length) {
        const auto sound{static_cast<Device*>(device)};
        sound->HandleAudioEvent(std::span(stream, length));
    }

    Device::Device(const i32 device) : recorder(device) {
        const auto devices{SDL_GetNumAudioDevices(device)};

        for (i32 out{}; out < devices; out++) {
            const auto devname{SDL_GetAudioDeviceName(out, recorder)};
            std::print("Available sound {} device: {}\n", recorder ? "input" : "output", devname);
            available.push_back(devname);
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

#if 1
        assert(specs.callback == nullptr);
        specs.callback = RenderSound;
        specs.userdata = this;
#endif

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
