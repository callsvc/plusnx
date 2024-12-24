#pragma once

#include <types.h>
namespace Plusnx::Armored::Ir {
    enum class IrOpcode {
        None,
        IrNop,
        IrReturn
    };

    class IrValue {
    public:
        IrValue(const IrOpcode opcode) : type(opcode) {}
        std::string to_string() const;
        IrOpcode type;

        operator bool() const {
            return type != IrOpcode::None;
        }
    };
}