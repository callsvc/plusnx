#pragma once

#include <types.h>
namespace Plusnx::Input {
    class FrontendUserControllerDriver {
    public:
        virtual ~FrontendUserControllerDriver() = default;

        virtual void PickAJoystick() = 0;
    };
}