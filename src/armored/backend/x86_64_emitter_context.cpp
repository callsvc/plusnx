#include <armored/backend/x86_64_emitter_context.h>

namespace Plusnx::Armored::Backend {
    void X86_64_EmitterContext::Compile([[maybe_unused]] const std::list<std::unique_ptr<Ir::IrDescriptor>>& list) {
    }

    void X86_64_EmitterContext::EmitNop() {
        lastInstructionSize = 1;
        WriteInstruction<u8>(0x90);
    }
}
