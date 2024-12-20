#pragma once

#include <armored/ir/ir_types.h>
namespace Plusnx::Armored::Ir {
    class IrConstructor {
    public:
        void Nop(const u8* addr);

        std::map<const u8*, IrOpcode> irs;
    };

    class IrDescriptor {
    public:
        IrDescriptor(const u8* pc, const u64 count) {
            a64pcVma = std::span(pc, count * 4);
        }

        IrDescriptor(IrDescriptor&& move) noexcept {
            a64pcVma = move.a64pcVma;
            list.irs = std::move(move.list.irs);
        }
        std::span<const u8> a64pcVma{};
        IrConstructor list;
    };
}