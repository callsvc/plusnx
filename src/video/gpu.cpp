#include <video/gpu.h>

namespace Plusnx::Video {
    GPU::GPU() : instance(std::make_unique<Vk::Instance>()) {
    }
}
