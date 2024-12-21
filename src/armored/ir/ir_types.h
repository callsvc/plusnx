#pragma once

#include <types.h>
namespace Plusnx::Armored::Ir {
    enum class IrOpcode {
        None,
        IrNop
    };

    class IrType {
    public:
        std::string to_string() const;
        IrOpcode type;

        operator bool() const {
            return type != IrOpcode::None;
        }
    };
}