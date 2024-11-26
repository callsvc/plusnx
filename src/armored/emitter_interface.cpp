#include <armored/code_blocks.h>
#include <armored/emitter_interface.h>

namespace Plusnx::Armored {
    void EmitterInterface::ChangeBlockScheme(const std::shared_ptr<CodeBlocks>& cb) {
        blocks = cb;
        pc = {};
    }

    void EmitterInterface::WriteI(const std::span<u8>& instruction) {
        auto section{static_cast<u8*>(blocks.lock()->executable) + pc};

        assert(instruction.size() == details->GetInstructionSize());

        for (const auto part : instruction)
            *section++ = part;
        AdvancePc();
    }

    void EmitterInterface::AdvancePc() {
        pc += details->GetInstructionSize();
    }
}
