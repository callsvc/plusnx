#include <armored/backend/emitter_generator.h>

#include <armored/readable_text_block.h>
namespace Plusnx::Armored::Backend {
    void EmitterGenerator::PushCpuContext(CpuContext& core, const std::shared_ptr<ReadableTextBlock>& from, const u64 size) {
        cpus.emplace_back(core, from);

        if (!blocks.expired())
            contexts.emplace(mapping, blocks);

        blocks = cpus.back().second;
        mapping = {};

        from->Initialize(size);
    }

    void EmitterGenerator::PopCpuContext(const CpuContext& core) {
        auto it{cpus.begin()};
        for (; it != cpus.end(); ++it)
            if (it->first.ccid == core.ccid)
                break;
        mapping = [&] -> u64 {
            for (const auto& [map, block] : contexts) {
                if (block == it->second)
                    mapping = map;
            }
            return {};
        }();
        blocks = it->second;
        cpus.erase(it);
    }

    void EmitterGenerator::WriteInstruction(const std::span<u8>& instruction) {
        auto image = [&] -> u8* {
            if (const std::shared_ptr executable{blocks})
                return &executable->text[mapping];
            return {};
        }();
        assert(instruction.size() == backing->GetInstructionSize(true));
        for (const auto bytes : instruction) {
            *image++ = bytes;
        }

        Advance();
    }

    bool EmitterGenerator::IsCompiled(const std::list<std::unique_ptr<Ir::IrDescriptor>>& is) const {
        assert(is.size());
        assert(cpus.size());
        return false;
    }

    u64 EmitterGenerator::Execute() const {
        assert(cpus.size());
        return {};
    }

    void EmitterGenerator::Advance() {
        mapping += backing->GetInstructionSize(true);
    }
}
