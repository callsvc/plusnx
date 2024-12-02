#pragma once
#include <armored/emitter_details.h>
namespace Plusnx::Armored {
    class Arm64Details final : public EmitterDetails {
    public:
        u64 GetInstructionSize() override;
    };
}
