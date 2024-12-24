#include <ranges>

#include <armored/readable_text_block.h>
#include <armored/backend/x86_64_emitter_context.h>
namespace Plusnx::Armored::Backend {
    void X86_64_EmitterContext::Compile(const std::list<Ir::IrDescriptorFlowGraph>& list) {

        const auto context{GetThreadCtx()};
        context->segment->Protect(TextProtectionStatus::Disable);
        for (const auto& cfgs : list) {
            cfgs.Foreach([&] (const auto& irOpc) {
                if (irOpc.type == Ir::IrOpcode::IrNop)
                    EmitNop();

                const auto result{x86Dism.to_string(std::span(lastInstruction))};
                if (!result.empty())
                    std::println("IR.X86_64 -> {}", result.front());
            });
        }
        context->segment->Protect(TextProtectionStatus::Enable);
    }

    void X86_64_EmitterContext::EmitNop() {
        WriteInstruction<u8>(0x90);
    }
}
