#include <print>

#include <SDL2/SDL.h>

#include <audio/speaker.h>
namespace Plusnx::Audio {
    using namespace std::literals;
    constexpr std::array desired{
        "pipewire"sv
    };

    Speaker::Speaker() {
        if (!ContainsValue(desired, std::string_view(SDL_GetCurrentAudioDriver())))
            MigrateDriver();

        std::print("Current audio driver: {}\n", SDL_GetCurrentAudioDriver());

        const auto devices{SDL_GetNumAudioDevices(0)};

        for (i32 out{}; out < devices; out++) {
            std::print("Available sound output device: {}\n", SDL_GetAudioDeviceName(out, 0));
        }
        SDL_AudioSpec current;
        SDL_GetDefaultAudioInfo(&device, &current, 0);
        if (device)
            assert(std::string_view(SDL_GetAudioDeviceName(0, 0)) == device);

        audio = SDL_OpenAudioDevice(device, 0, &current, nullptr, 0);
    }

    Speaker::~Speaker() {
        SDL_CloseAudioDevice(audio);
        SDL_free(device);
        if (driver)
            SDL_AudioQuit();
    }

    void Speaker::MigrateDriver() {
        const auto drivers{SDL_GetNumAudioDrivers()};
        for (i32 drv{}; drv < drivers && !audio; drv++) {
            const auto name{SDL_GetAudioDriver(drv)};
            if (ContainsValue(desired, std::string_view(name))) {
                SDL_SetHintWithPriority("SDL_AUDIO_DRIVER", name, SDL_HINT_OVERRIDE);
                driver = SDL_AudioInit(name) == 0;
            }
        }
    }
}
