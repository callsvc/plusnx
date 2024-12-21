#include <armored/backend/emitter_generator.h>

#include <armored/frontend/arm64_translator.h>
namespace Plusnx::Armored::Frontend {
    u64 Arm64Translator::GetInstructionSize(const bool fromHost) {
        if (fromHost) {
            if (const std::shared_ptr emitter{parent})
                return emitter->lastInstructionSize;
        }

        if (thumb)
            return 2;
        return 4;
    }

    void Arm64Translator::Translate(const u8* pc, u64 count) {
        for (const auto& irDescriptor : irsList) {
            if (pc >= irDescriptor->a64pcVma.data() &&
                pc < irDescriptor->a64pcVma.end().base())
                return;
        }
        auto descriptor{std::make_unique<Ir::IrDescriptorFlowGraph>(pc, count)};
        for (; count-- > 0; pc += 4) {
            u32 instruction{};
            std::memcpy(&instruction, pc, sizeof(4));
            std::println("(Converting instruction) {}", armDism.to_string(instruction));
            descriptor->Nop(pc);
        }

        irsList.emplace_back(std::move(descriptor));
    }
}
