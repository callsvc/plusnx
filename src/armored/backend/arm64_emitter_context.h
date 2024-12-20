#pragma once
#include <armored/backend/emitter_generator.h>
namespace Plusnx::Armored::Backend {
    class Arm64EmitterContext final : public EmitterGenerator {
    public:
        explicit Arm64EmitterContext(const std::shared_ptr<EmitterInterface>& interface);
        void Compile(const std::list<std::unique_ptr<Ir::IrDescriptor>>& list) override;
        void EmitNop() override;
    };
}
