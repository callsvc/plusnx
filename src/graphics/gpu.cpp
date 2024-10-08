#include <graphics/gpu.h>

namespace Plusnx::Graphics {
    GPU::GPU() : instance(std::make_unique<Vk::Instance>()) {
    }
}
