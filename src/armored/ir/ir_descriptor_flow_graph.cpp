#include <ranges>
#include <armored/ir/ir_descriptor_flow_graph.h>

namespace Plusnx::Armored::Ir {
    void IrDescriptorFlowGraph::StartBlock() {
        if (!graph.empty()) {
            irs.emplace(scopes.back(), std::move(graph));
        }
        scopes.push_back(a64pcVma.data() + graph.size() * 4);
        graph.clear();
    }

    void IrDescriptorFlowGraph::EndBlock() {
        if (graph.back().type != IrOpcode::IrReturn) {
            return;
        }
        irs.emplace(scopes.back(), std::move(graph));
        scopes.pop_back();
    }

    void IrDescriptorFlowGraph::Foreach(std::function<void(const IrValue&)>&& callback) const {
        auto ForeachValues = [&](const IrDescriptorFlowGraph& graph) {
            for (const auto& values : graph.irs | std::ranges::views::values) {
                for (const auto& irVal : values) {
                    callback(irVal);
                }
            }
        };
        ForeachValues(*this);
        for (const auto& flows : flow | std::views::values) {
            ForeachValues(flows);
        }

    }

    const u8* IrDescriptorFlowGraph::FirstPc() const {
        if (!irs.empty())
            return irs.begin()->first;
        if (!flow.empty())
            return flow.begin()->first;
        return nullptr;
    }
    const u8* IrDescriptorFlowGraph::LastPc() const {
        if (!flow.empty())
            return std::prev(flow.end())->first;
        if (!irs.empty())
            return std::prev(irs.end())->first;
        return nullptr;
    }

    void IrDescriptorFlowGraph::Nop() {
        graph.emplace_back(IrOpcode::IrNop);
    }
    void IrDescriptorFlowGraph::Return() {
        graph.emplace_back(IrOpcode::IrReturn);
    }
}
