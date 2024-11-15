#include <print>
#include <audio/sound.h>
#include <SDL2/SDL_hints.h>

namespace Plusnx::Audio {
    static std::vector<std::string_view> soundDrivers{};

    Sound::Sound() {
#if defined(__linux__)
        soundDrivers.emplace_back("pipewire");
        soundDrivers.emplace_back("pulseaudio");
#endif
        MigrateDriver();

        std::print("Current audio driver: {}\n", SDL_GetCurrentAudioDriver());

        output = std::make_unique<OutputDevice>();
        input = std::make_unique<InputDevice>();
    }

    Sound::~Sound() {
        input->Stop();
        output->Stop();
        if (driver)
            SDL_AudioQuit();
    }

    void Sound::MigrateDriver() {
        const auto drivers{SDL_GetNumAudioDrivers()};
        for (i32 drv{}; drv < drivers && !driver; drv++) {
            const auto name{SDL_GetAudioDriver(drv)};
            if (ContainsValue(soundDrivers, std::string_view(name))) {
                SDL_SetHintWithPriority("SDL_AUDIO_DRIVER", name, SDL_HINT_OVERRIDE);
                driver = SDL_AudioInit(name) == 0;
            }
        }
    }
}
