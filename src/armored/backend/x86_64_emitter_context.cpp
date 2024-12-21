#include <ranges>

#include <armored/readable_text_block.h>
#include <armored/backend/x86_64_emitter_context.h>
#include <boost/container/small_vector.hpp>
namespace Plusnx::Armored::Backend {
    void X86_64_EmitterContext::Compile(const std::list<std::unique_ptr<Ir::IrDescriptorFlowGraph>>& list) {

        const auto context{GetThreadCtx()};
        boost::container::small_vector<u8, 16> instruction(16);
        context->segment->Protect(TextProtectionStatus::Disable);
        for (const auto& flows : list) {
            for (const auto& [origin, opcode] : flows->irs) {
                assert(origin && static_cast<bool>(opcode));
                EmitNop();

                std::memcpy(instruction.data(), context->next - lastInstructionSize, lastInstructionSize);
                if (const auto result{x86Dism.to_string(std::span(instruction))}; !result.empty())
                    std::println("Generated instruction: {}", result.front());
            }
        }
        context->segment->Protect(TextProtectionStatus::Enable);
    }

    void X86_64_EmitterContext::EmitNop() {
        lastInstructionSize = 1;
        WriteInstruction<u8>(0x90);
    }
}
