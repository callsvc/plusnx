#include <armored/backend/x86_64_emitter_context.h>

namespace Plusnx::Armored::Backend {
    void X86_64_EmitterContext::EmitNop() {
        lastInstructionSize = 1;
        WriteI<u8>(0x90);
    }
}
