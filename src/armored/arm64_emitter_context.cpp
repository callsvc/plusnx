#include <armored/arm64_emitter_context.h>

void Plusnx::Armored::Arm64EmitterContext::EmitNop() {
    WriteI<u32>(0xD503201F);
}
