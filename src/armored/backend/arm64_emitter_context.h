#pragma once
#include <armored/backend/emitter_generator.h>
namespace Plusnx::Armored::Backend {
    class Arm64EmitterContext final : public EmitterGenerator {
    public:
        explicit Arm64EmitterContext(const std::shared_ptr<EmitterInterface>& interface);
        void EmitNop() override;
    };
}
