#include <armored/backend/emitter_generator.h>

#include <armored/frontend/arm64_translator.h>
namespace Plusnx::Armored::Frontend {
    u64 Arm64Translator::GetInstructionSize(const bool fromHost) {
        if (fromHost) {
            if (const std::shared_ptr emitter{parent})
                return emitter->lastInstructionSize;
        }

        if (thumb)
            return 2;
        return 4;
    }
}
