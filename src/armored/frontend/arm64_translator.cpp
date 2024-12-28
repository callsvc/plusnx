#include <armored/backend/emitter_generator.h>

#include <armored/frontend/info_target_instructions.h>
#include <armored/frontend/arm64_translator.h>
namespace Plusnx::Armored::Frontend {
    u64 Arm64Translator::GetInstructionSize(const bool fromHost) {
        if (fromHost) {
            if (const std::shared_ptr emitter{parent})
                return emitter->lastInstruction.size();
        }

        if (thumb)
            assert(0);
        return 4;
    }
    void Arm64Translator::Translate(const u8* pc, const u64 count) {
        const auto instructions{CreateSpanType<const u32>(pc, count)};

        for (const auto& descriptor : irsList) {
            const auto end{descriptor.flow.end()};
            if (pc >= descriptor.flow.begin()->first &&
                pc + count * 4 <= end->second.a64pcVma.end().base()) {
                return;
            }
        }

        Ir::IrDescriptorFlowGraph cfg{pc, count};
        for (const auto arm : instructions) {
            if (!arm)
                break;
            if (!cfg)
                cfg.StartBlock();

            std::println("ARM.IR -> {}", armDism.to_string(arm));

            if (const auto result{farm64idec.Decode(arm)}) {
                [[maybe_unused]] const auto instruction{result->type};
            } else {
                cfg.Nop();
            }

            if (IsArm64Ret(arm)) {
                cfg.Return();
            }

            if (IrArm64BranchFlavour(arm)) {
                cfg.EndBlock();
            }
            pc += GetInstructionSize(false);
        }

        irsList.emplace_back(std::move(cfg));
    }
}
