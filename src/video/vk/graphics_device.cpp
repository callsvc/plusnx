#include <map>
#include <ranges>
#include <video/vk/graphics_device.h>
namespace Plusnx::Video::Vk {
    GraphicsDevice::GraphicsDevice(const std::shared_ptr<AppInstance>& app) : vkInstance(app->vkInstance) {
        const auto devices{vkInstance.enumeratePhysicalDevices()};
        std::multimap<i32, VkPhysicalDevice, std::less<>> selection;

        for (const auto& [index, dev] : std::ranges::views::enumerate(devices)) {
            i32 counter{};
            if (dev.getProperties().deviceType == vk::PhysicalDeviceType::eDiscreteGpu) {
                counter += 1000;
            }
            if (dev.getFeatures().geometryShader)
                counter += 100;

            const auto queueInfos{dev.getQueueFamilyProperties()};
            for ([[maybe_unused]] const auto& queue : queueInfos) {
            }
            selection.insert(std::make_pair(counter, dev));
        }

        if ([[maybe_unused]] auto picked = selection.begin()->first) {
        }
    }
}
