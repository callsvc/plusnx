#pragma once
#include <armored/backend/emitter_generator.h>

namespace Plusnx::Armored::Backend {
    class X86_64_EmitterContext final : public EmitterGenerator {
    public:
        X86_64_EmitterContext(const std::shared_ptr<EmitterInterface>& interface) : EmitterGenerator(interface) {
        }

        void Compile(const std::list<std::unique_ptr<Ir::IrDescriptor>>& list) override;
        void EmitNop() override;
    };
}
