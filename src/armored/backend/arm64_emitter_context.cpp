#include <armored/backend/arm64_emitter_context.h>

namespace Plusnx::Armored::Backend {
    Arm64EmitterContext::Arm64EmitterContext(const std::shared_ptr<EmitterInterface>& interface) : EmitterGenerator(interface) {
        lastInstructionSize = 4; // Always 4
    }

    void Arm64EmitterContext::EmitNop() {
        WriteI<u32>(0xD503201F);
    }
}
