#include <armored/ir/ir_descriptor_flow_graph.h>

namespace Plusnx::Armored::Ir {
    void IrDescriptorFlowGraph::Nop(const u8* addr) {
        irs.emplace(addr, IrOpcode::IrNop);
    }
}
