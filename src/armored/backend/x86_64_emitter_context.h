#pragma once
#include <armored/arm_x86_disas.h>
#include <armored/backend/emitter_generator.h>

namespace Plusnx::Armored::Backend {
    class X86_64_EmitterContext final : public EmitterGenerator {
    public:
        X86_64_EmitterContext(const std::shared_ptr<EmitterInterface>& interface) : EmitterGenerator(interface) {
        }

        void Compile(const std::list<std::unique_ptr<Ir::IrDescriptorFlowGraph>>& list) override;

        ArmX86Disas x86Dism{DisasFlavourType::X86_64};
        void EmitNop() override;
    };
}
