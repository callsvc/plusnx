#include <armored/ir/ir_types.h>

namespace Plusnx::Armored::Ir {
    std::string IrValue::to_string() const {
        if (type == IrOpcode::IrNop)
            return "IR0_NO_OPERATION";

        switch (type) {
            case IrOpcode::IrReturn:
                return "IR0_RETURN";
            default:
                return "UNKNOWN";
        }
    }
}
