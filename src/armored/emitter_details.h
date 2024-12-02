#pragma once

#include <armored/arm_types.h>
namespace Plusnx::Armored {
    class EmitterDetails {
    public:
        virtual ~EmitterDetails() = default;
        virtual u64 GetInstructionSize() = 0;
    };
}