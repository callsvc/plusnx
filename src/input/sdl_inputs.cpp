#include <cassert>
#include <SDL2/SDL.h>

#include <input/sdl_inputs.h>
namespace Plusnx::Input {
    SdlInputs::SdlInputs() {
        SDL_SetHint(SDL_HINT_JOYSTICK_HIDAPI_PS4_RUMBLE, "1");
        SDL_SetHint(SDL_HINT_JOYSTICK_HIDAPI_PS5_RUMBLE, "1");

        SDL_SetHint(SDL_HINT_JOYSTICK_HIDAPI_SWITCH, "1");
        SDL_SetHint(SDL_HINT_JOYSTICK_HIDAPI_JOY_CONS, "1");

        assert(SDL_InitSubSystem(SDL_INIT_JOYSTICK) == 0);
    }

    void SdlInputs::PickAJoystick() {
        for (i32 joy{}; joy < SDL_NumJoysticks(); joy++) {
        }
    }
}
