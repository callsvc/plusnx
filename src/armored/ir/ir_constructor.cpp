#include <armored/ir/ir_constructor.h>

namespace Plusnx::Armored::Ir {
    void IrConstructor::Nop(const u8* addr) {
        irs.emplace(addr, IrOpcode::IrNop);
    }
}
