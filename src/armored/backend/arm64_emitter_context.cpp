#include <armored/frontend/mach_arm64_disas.h>
#include <armored/backend/arm64_emitter_context.h>

namespace Plusnx::Armored::Backend {
    Arm64EmitterContext::Arm64EmitterContext(const std::shared_ptr<EmitterInterface>& interface) : EmitterGenerator(interface) {
        lastInstructionSize = 4; // Always 4
    }

    void Arm64EmitterContext::Compile([[maybe_unused]] const std::list<std::unique_ptr<Ir::IrDescriptor>>& list) {
        std::terminate();
    }

    void Arm64EmitterContext::EmitNop() {
        WriteInstruction<u32>(std::to_underlying(Frontend::OpcodeArm64::NoOperation));
    }
}
