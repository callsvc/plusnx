#pragma once

#include <memory>

#include <video/vk/instance.h>
namespace Plusnx::Video {
    class GPU {
    public:
        GPU();
        std::unique_ptr<Vk::Instance> instance;
    };
}