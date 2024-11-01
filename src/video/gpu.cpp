#include <video/gpu.h>

namespace Plusnx::Video {
    void GPU::Initialize(const Vk::VkSupport& support) {
        std::vector<std::string_view> ext;
        ext.reserve(support.enableExt.size());
        for (const auto extension : support.enableExt) {
            ext.emplace_back(extension);
        }
        try {
            instance = std::make_shared<Vk::AppInstance>(ext);
            device = std::make_shared<Vk::GraphicsDevice>(instance);
        } catch ([[maybe_unused]] std::exception& except) {

        }
    }
}
