#pragma once

#include <armored/arm_types.h>
namespace Plusnx::Armored::Backend {
    class EmitterGenerator;

    class EmitterInterface {
    public:
        void Initialize(const std::shared_ptr<EmitterGenerator>& generator) {
            parent = generator;
        }

        virtual ~EmitterInterface() = default;
        virtual u64 GetInstructionSize(bool fromHost = false) = 0;

        std::weak_ptr<EmitterGenerator> parent;
    };
}