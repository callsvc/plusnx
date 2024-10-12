#pragma once

#include <memory>

#include <sys_fs/assets.h>
namespace Plusnx {
    class Context;

    class Application {
    public:
        Application();

        std::shared_ptr<Context> context;
        std::shared_ptr<SysFs::Assets> assets;
    };
}
