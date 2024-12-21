#include <ranges>
#include <armored/backend/emitter_generator.h>

#include <armored/readable_text_block.h>
namespace Plusnx::Armored::Backend {
    void EmitterGenerator::Activate(
        const CpuContext& core,
        const std::shared_ptr<ReadableTextBlock>& textbl,
        const u64 size) {

        const auto number{core.identifier};

        if (cpus.contains(number)) {
            if (cpus[number]->cpuCtx->owner == std::this_thread::get_id())
                cpus.erase(number);
        }

        cpus.insert_or_assign(number, std::make_shared<EmitterThreadContext>(core, textbl));
        textbl->Initialize(size);
    }

    std::shared_ptr<EmitterThreadContext> EmitterGenerator::GetThreadCtx() {
        for (const auto& contexts : std::ranges::views::values(cpus)) {
            if (contexts->cpuCtx->owner == std::this_thread::get_id())
                return contexts;
        }
        return {};
    }

    void EmitterGenerator::WriteInstruction(const std::span<u8>& instruction) {
        const auto context{GetThreadCtx()};
        if (!context)
            return;

        assert(instruction.size() == backing->GetInstructionSize(true));
        for (const auto [index, bytes] : std::views::enumerate(instruction)) {
            context->next[index] = bytes;
        }

        Advance(context);
    }

    bool EmitterGenerator::IsCompiled(const std::list<std::unique_ptr<Ir::IrDescriptorFlowGraph>>& is) const {
        u64 match{};
        for (const auto& contexts : std::ranges::views::values(cpus)) {
            for (const auto& _irs : is) {
                if (contexts->reflect.data() == _irs->irs.begin()->first)
                    match++;

                const auto lastIrs{std::prev(_irs->irs.end())};
                if (contexts->reflect.end().base() == lastIrs->first + 4)
                    break;
            }
        }
        return match == is.size();
    }

    u64 EmitterGenerator::Execute() const {
        assert(cpus.size());
        return {};
    }

    void EmitterGenerator::ResetBuffer() {
        const auto context{GetThreadCtx()};
        if (!context)
            return;
        const auto* last{context->next};
        context->next = context->segment->text;
        assert(context->next != last);
    }

    void EmitterGenerator::Advance(const std::shared_ptr<EmitterThreadContext>& context) const {
        context->next += backing->GetInstructionSize(true);
        const auto* end{context->segment->text + context->segment->size};
        if (context->next >= end) {
            context->segment->Expand();
        }
    }
}
