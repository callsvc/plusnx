#pragma once

#include <input/controller_types.h>
namespace Plusnx::Input {
    class SdlInputs final : public FrontendUserControllerDriver {
    public:
        SdlInputs();

        void PickAJoystick() override;

        std::vector<const char*> joysName;
    };
}