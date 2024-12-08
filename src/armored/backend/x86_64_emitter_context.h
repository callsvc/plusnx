#pragma once
#include <armored/backend/emitter_generator.h>

namespace Plusnx::Armored::Backend {
    class X86_64_EmitterContext final : public EmitterGenerator {
    public:
        X86_64_EmitterContext(const std::shared_ptr<EmitterInterface>& interface) : EmitterGenerator(interface) {
        }

        void EmitNop() override;
    };
}
