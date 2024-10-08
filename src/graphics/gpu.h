#pragma once

#include <memory>

#include <graphics/vk/instance.h>
namespace Plusnx::Graphics {
    class GPU {
    public:
        GPU();
        std::unique_ptr<Vk::Instance> instance;
    };
}