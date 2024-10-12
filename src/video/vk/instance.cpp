#include <print>

#include <video/vk/instance.h>
namespace Plusnx::Video::Vk {
    Instance::Instance() {
        std::vector<const char*> layerNames;

        u32 layersCount;

        if (vk::enumerateInstanceLayerProperties(&layersCount, nullptr) != vk::Result::eSuccess)
            throw std::runtime_error("VK: Failed to capture the supported layers");

        if (layersCount) {
            std::vector<vk::LayerProperties> availableLayers(layersCount);
            if (enumerateInstanceLayerProperties(&layersCount, availableLayers.data()) == vk::Result::eSuccess)
                std::print("{} layers found", availableLayers.size());

            for (const auto& layer : availableLayers) {
                if (layer.layerName == "VK_LAYER_KHRONOS_validation")
                    layerNames.emplace_back("VK_LAYER_KHRONOS_validation");
            }
        }

        vk::ApplicationInfo applicationInfo{"Plusnx", VK_MAKE_VERSION(1, 3, 0)};
        const vk::InstanceCreateInfo instanceCreateInfo{
            {},
            &applicationInfo,
            static_cast<u32>(layerNames.size()),
            layerNames.data()
        };

        context.emplace(createInstance(instanceCreateInfo));
    }

    Instance::~Instance() {
        if (context)
            vkDestroyInstance(context.value(), nullptr);
    }
}
