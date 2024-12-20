#pragma once
#include <list>

#include <armored/backend/emitter_interface.h>
#include <armored/cpu_context.h>
namespace Plusnx::Armored {
    class ReadableTextBlock;
}

namespace Plusnx::Armored::Backend {
    class EmitterGenerator {
    public:
        explicit EmitterGenerator(const std::shared_ptr<EmitterInterface>& interface) : backing(interface) {}
        virtual ~EmitterGenerator() = default;

        void PushCpuContext(CpuContext& core, const std::shared_ptr<ReadableTextBlock>& from, u64 size);
        void PopCpuContext(const CpuContext& core);

        void WriteInstruction(const std::span<u8>& instruction);

        template <typename T> requires (std::is_integral_v<T>)
        void WriteInstruction(T instruction) {
            WriteInstruction(std::span(reinterpret_cast<u8*>(&instruction), sizeof(T)));
        }

        bool IsCompiled(const std::list<std::unique_ptr<Ir::IrDescriptor>>& is) const;
        virtual void Compile(const std::list<std::unique_ptr<Ir::IrDescriptor>>& list) = 0;

        u64 Execute() const;

        u64 mapping{};
        u64 lastInstructionSize{1}; // Assuming the first instruction is a single-byte NOP
        std::weak_ptr<ReadableTextBlock> blocks;
        std::shared_ptr<EmitterInterface> backing;

        std::vector<std::pair<CpuContext, std::shared_ptr<ReadableTextBlock>>> cpus;

        virtual void EmitNop() = 0;
    private:
        void Advance();

        std::map<u64, std::shared_ptr<ReadableTextBlock>> contexts;
    };
}
