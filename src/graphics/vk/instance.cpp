#include <graphics/vk/instance.h>

namespace Plusnx::Graphics::Vk {
    Instance::Instance() {
        std::vector layerNames{
            "VK_LAYER_KHRONOS_validation"
        };

        vk::ApplicationInfo applicationInfo{"plusnx"};
        const vk::InstanceCreateInfo instanceCreateInfo{
            {},
            &applicationInfo,
            static_cast<u32>(layerNames.size()),
            layerNames.data()
        };

        context.emplace(createInstance(instanceCreateInfo));
    }
}
