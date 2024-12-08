#pragma once
#include <list>

#include <armored/backend/emitter_interface.h>
#include <armored/cpu_context.h>
namespace Plusnx::Armored {
    class CodeBlocks;
}

namespace Plusnx::Armored::Backend {
    class EmitterGenerator {
    public:
        explicit EmitterGenerator(const std::shared_ptr<EmitterInterface>& interface) : backing(interface) {}
        virtual ~EmitterGenerator() = default;

        void PushCpuContext(CpuContext& core, const std::shared_ptr<CodeBlocks>& blocks, u64 size);
        void PopCpuContext(const CpuContext& core);

        void WriteI(const std::span<u8>& instruction);
        template <typename T>
        void WriteI(T instruction) {
            WriteI(std::span(reinterpret_cast<u8*>(&instruction), sizeof(T)));
        }

        u64 codeMap{};
        u64 lastInstructionSize{1}; // Assuming the first instruction is a single-byte NOP
        std::weak_ptr<CodeBlocks> blockMap;
        std::shared_ptr<EmitterInterface> backing;

        virtual void EmitNop() = 0;
    private:
        void Advance();

        std::vector<std::pair<CpuContext, std::shared_ptr<CodeBlocks>>> cpus;
        std::map<u64, std::shared_ptr<CodeBlocks>> contexts;
    };
}
