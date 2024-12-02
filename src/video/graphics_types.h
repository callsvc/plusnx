#pragma once

#include <vulkan/vulkan.hpp>
namespace Plusnx::Video {
    enum class BackendType {
        Vulkan,
        OpenGL
    };
    enum class ApiType {
        None,
        Sdl,
    };
    class GraphicsSupportContext {
    public:
        virtual ~GraphicsSupportContext() = default;
        virtual void ActivateContext(const vk::Instance& context) = 0;
        virtual void Update() = 0;

        std::vector<const char*> required;
        BackendType type;
        ApiType apiType;
        VkSurfaceKHR surface;
        vk::Instance instance;
    };
}