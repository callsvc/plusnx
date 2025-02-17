#pragma once

#include <armored/ir/ir_descriptor_flow_graph.h>
#include <armored/arm_types.h>
namespace Plusnx::Armored::Backend {
    class EmitterGenerator;

    class EmitterInterface {
    public:
        virtual ~EmitterInterface() = default;
        virtual u64 GetInstructionSize(bool fromHost = false) = 0;
        virtual void Translate(const u8* pc, u64 count) = 0;

        virtual std::list<Ir::IrDescriptorFlowGraph>& GetList() = 0;

        std::weak_ptr<EmitterGenerator> parent;
    };
}
