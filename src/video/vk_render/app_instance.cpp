#include <print>

#include <types.h>
#include <video/vk_render/app_instance.h>
namespace Plusnx::Video::VkRender {
    void AppInstance::EnumerateSupportLayers() {
        u32 layersCount;

        if (vk::enumerateInstanceLayerProperties(&layersCount, nullptr) != vk::Result::eSuccess)
            throw exception("VK: Failed to capture the supported layers");

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
    void AppInstance::EnumerateRequiredExtensions(const std::vector<std::string_view>& required) {
        u32 count;
        assert(vk::enumerateInstanceExtensionProperties(nullptr, &count, nullptr) == vk::Result::eSuccess);
        std::vector<vk::ExtensionProperties> properties(count);
        std::print("Number of extensions supported by Vulkan 1.3: {}\n", count);

        assert(vk::enumerateInstanceExtensionProperties(nullptr, &count, properties.data()) == vk::Result::eSuccess);

        for (const auto& property : properties) {
            for (const auto& content : required) {
                if (content == std::string_view(property.extensionName))
                    extensions.emplace_back(content.data());
            }
        }
    }


    AppInstance::AppInstance(const std::vector<std::string_view>& required) {
        EnumerateSupportLayers();
        EnumerateRequiredExtensions(required);

        vk::ApplicationInfo applicationInfo{"Plusnx", VK_API_VERSION_1_3};
        const vk::InstanceCreateInfo instanceCreateInfo{
            {}, &applicationInfo, static_cast<u32>(layers.size()), layers.data(), static_cast<u32>(extensions.size()), extensions.data()
        };

        assert(required.size() == extensions.size());
        instance = createInstance(instanceCreateInfo);
    }

    AppInstance::~AppInstance() {
        if (instance)
            vkDestroyInstance(instance, nullptr);
    }
}
