#pragma once
#include <armored/arm_x86_disas.h>
#include <armored/backend/emitter_interface.h>
#include <armored/ir/ir_descriptor_flow_graph.h>

#include <armored/frontend/arm64_faster_intrusive_decoder.h>
namespace Plusnx::Armored::Frontend {
    class Arm64Translator final : public Backend::EmitterInterface {
    public:
        u64 GetInstructionSize(bool fromHost) override;
        void Translate(const u8* pc, u64 count) override;

        std::list<Ir::IrDescriptorFlowGraph>& GetList() override {
            return irsList;
        }

        bool thumb{false};

        ArmX86Disas armDism{DisasFlavourType::Arm64};
        FasterArm64IntrusiveDecoder farm64idec;
        std::list<Ir::IrDescriptorFlowGraph> irsList;
    };
}
