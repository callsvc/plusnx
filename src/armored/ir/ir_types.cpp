#include <armored/ir/ir_types.h>

namespace Plusnx::Armored::Ir {
    std::string IrType::to_string() const {
        if (type == IrOpcode::IrNop)
            return "NOP";
        return {};
    }
}
