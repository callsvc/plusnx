#include <ranges>
#include <armored/backend/x86_64_emitter_context.h>
#include <boost/container/small_vector.hpp>

namespace Plusnx::Armored::Backend {
    void X86_64_EmitterContext::Compile(const std::list<std::unique_ptr<Ir::IrDescriptorFlowGraph>>& list) {

        boost::container::small_vector<u8, 16> instruction;
        for (const auto& flows : list) {
            for (const auto& [origin, opcode] : flows->irs) {
                assert(origin && static_cast<bool>(opcode));
                EmitNop();

                const auto* begin{mapping -= lastInstructionSize};
                std::memcpy(instruction.data(), begin, lastInstructionSize);

                std::println("Generated instruction: {}", x86Dism.to_string(std::span(instruction)));
            }
        }
    }

    void X86_64_EmitterContext::EmitNop() {
        lastInstructionSize = 1;
        WriteInstruction<u8>(0x90);
    }
}
