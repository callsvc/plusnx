#pragma once

#include <vector>

#include <SDL2/SDL.h>
namespace Plusnx::NoGui {
    class SdlVulkanBacking {
    public:
        SdlVulkanBacking();
        ~SdlVulkanBacking();

        std::vector<const char*> required;
        SDL_Window* window{nullptr};
    };
}