#pragma once
#include <armored/backend/emitter_interface.h>
#include <armored/ir/ir_constructor.h>

namespace Plusnx::Armored::Frontend {
    class Arm64Translator final : public Backend::EmitterInterface {
    public:
        u64 GetInstructionSize(bool fromHost) override;
        void Translate(const u8* pc, u64 count) override;

        std::list<std::unique_ptr<Ir::IrDescriptor>>& GetList() override {
            return irsList;
        }

        bool thumb{false};

        std::list<std::unique_ptr<Ir::IrDescriptor>> irsList;
    };
}
