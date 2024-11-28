#include <cassert>
#include <SDL2/SDL.h>

#include <input/sdl_inputs.h>
namespace Plusnx::Input {
    SdlInputs::SdlInputs() {
        assert(SDL_Init(SDL_INIT_JOYSTICK) == 0);

    }

    void SdlInputs::PickAJoystick() {
        for (i32 joy{}; joy < SDL_NumJoysticks(); joy++) {
        }
    }
}
