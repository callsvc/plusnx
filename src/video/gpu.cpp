#include <video/gpu.h>

namespace Plusnx::Video {
    GPU::GPU() = default;

    void GPU::InitGraphics(const Vk::VkSupport& support) {
        std::vector<std::string_view> ext;
        ext.reserve(support.enableExt.size());
        for (const auto extension : support.enableExt) {
            ext.emplace_back(extension);
        }
        instance = std::make_unique<Vk::Instance>(ext);
    }
}
