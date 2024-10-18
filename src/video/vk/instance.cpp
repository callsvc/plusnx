#include <print>

#include <video/vk/instance.h>
#include <types.h>
namespace Plusnx::Video::Vk {
    void Instance::EnumerateSupportLayers() {
        u32 layersCount;

        if (vk::enumerateInstanceLayerProperties(&layersCount, nullptr) != vk::Result::eSuccess)
            throw std::runtime_error("VK: Failed to capture the supported layers");

        if (!layersCount)
            return;
        std::vector<vk::LayerProperties> availableLayers(layersCount);
        if (enumerateInstanceLayerProperties(&layersCount, availableLayers.data()) == vk::Result::eSuccess)
            std::print("{} layers found", availableLayers.size());

        for (const auto& layer : availableLayers) {
            if (std::string_view(layer.layerName) == "VK_LAYER_KHRONOS_validation")
                layers.emplace_back("VK_LAYER_KHRONOS_validation");
        }
    }
    void Instance::EnumerateRequiredExtensions(const std::vector<std::string_view>& required) {
        u32 count;
        assert(vk::enumerateInstanceExtensionProperties(nullptr, &count, nullptr) == vk::Result::eSuccess);
        std::vector<vk::ExtensionProperties> properties(count);
        std::print("Number of extensions supported by Vulkan 1.3: {}\n", count);

        assert(vk::enumerateInstanceExtensionProperties(nullptr, &count, properties.data()) == vk::Result::eSuccess);

        for (const auto& property : properties) {
            if (ContainsValue(required, std::string_view(property.extensionName)))
                extensions.emplace_back(property.extensionName);
        }
    }


    Instance::Instance(const std::vector<std::string_view>& required) {
        EnumerateSupportLayers();
        EnumerateRequiredExtensions(required);

        vk::ApplicationInfo applicationInfo{"Plusnx", VK_API_VERSION_1_3};
        const vk::InstanceCreateInfo instanceCreateInfo{
            {}, &applicationInfo, static_cast<u32>(layers.size()), layers.data(), static_cast<u32>(extensions.size()), extensions.data()
        };

        instance.emplace(createInstance(instanceCreateInfo));
    }

    Instance::~Instance() {
        if (instance)
            vkDestroyInstance(instance.value(), nullptr);
    }
}
