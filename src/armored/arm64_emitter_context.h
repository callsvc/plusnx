#pragma once
#include <armored/emitter_interface.h>
#include <armored/emitter_details.h>
namespace Plusnx::Armored {
    class Arm64EmitterContext final : public EmitterInterface {
    public:
        explicit Arm64EmitterContext(const std::shared_ptr<EmitterDetails>& platform)
            : EmitterInterface(platform) {
        }
        void EmitNop() override;
    };
}
