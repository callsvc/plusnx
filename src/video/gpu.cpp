#include <video/gpu.h>

namespace Plusnx::Video {
    void GPU::Initialize(const std::shared_ptr<GraphicsSupportContext>& support) {
        std::vector<std::string_view> extensions;
        extensions.reserve(support->required.size());
        for (const auto extension : support->required) {
            extensions.emplace_back(extension);
        }
        try {
            instance = std::make_shared<Vk::AppInstance>(extensions);
            device = std::make_shared<Vk::GraphicsDevice>(instance);
        } catch ([[maybe_unused]] const std::exception& except) {
            support->required.clear();
            std::rethrow_exception(std::current_exception());
        }

        support->ActivateContext(instance->instance);
    }
}
