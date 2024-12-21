#pragma once

#include <armored/ir/ir_types.h>
namespace Plusnx::Armored::Ir {
    class IrDescriptorFlowGraph {
    public:
        IrDescriptorFlowGraph(const u8* pc, const u64 count) {
            a64pcVma = std::span(pc, count * 4);
        }
        IrDescriptorFlowGraph(IrDescriptorFlowGraph&& irDfg) noexcept {
            a64pcVma = irDfg.a64pcVma;
            irs = std::move(irDfg.irs);
        }

        void Nop(const u8* addr);

        std::map<const u8*, IrOpcode> irs;
        std::span<const u8> a64pcVma{};
    };
}