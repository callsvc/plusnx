#pragma once
#include <armored/backend/emitter_interface.h>
namespace Plusnx::Armored::Frontend {
    class Arm64Translator final : public Backend::EmitterInterface {
    public:
        u64 GetInstructionSize(bool fromHost) override;

        bool thumb{false};
    };
}
