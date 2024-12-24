#pragma once
#include <deque>

#include <armored/ir/ir_types.h>
namespace Plusnx::Armored::Ir {
    class IrDescriptorFlowGraph {
    public:
        IrDescriptorFlowGraph(const u8* pc, const u64 count) {
            a64pcVma = std::span(pc, count * 4);
        }
        IrDescriptorFlowGraph(IrDescriptorFlowGraph&& block) noexcept {
            a64pcVma = block.a64pcVma;
            scopes = std::move(block.scopes);
            flow = std::move(block.flow);
            irs = std::move(block.irs);
        }

        operator bool() const {
            return scopes.size() && scopes.front() != nullptr;
        }

        void StartBlock();
        void EndBlock();
        void Foreach(std::function<void(const IrValue&)>&& callback) const;

        const u8* FirstPc() const;
        const u8* LastPc() const;

        void Nop();
        void Return();

        std::span<const u8> a64pcVma{}; // < Indicates where the current CFG was translated from

        std::deque<const u8*> scopes;
        std::vector<IrValue> graph;
        std::map<const u8*, IrDescriptorFlowGraph> flow;
    private:
        std::map<const u8*, std::vector<IrValue>> irs;
    };
}