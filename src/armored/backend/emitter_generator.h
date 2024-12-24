#pragma once
#include <list>

#include <boost/container/small_vector.hpp>

#include <armored/backend/emitter_interface.h>
#include <armored/cpu_context.h>
namespace Plusnx::Armored {
    class ReadableTextBlock;
}

namespace Plusnx::Armored::Backend {
    struct EmitterThreadContext {
        std::optional<CpuContext> cpuCtx;
        std::shared_ptr<ReadableTextBlock> segment;
        u8* next{nullptr};
        std::span<const u8> reflect{};
    };

    class EmitterGenerator {
    public:
        explicit EmitterGenerator(const std::shared_ptr<EmitterInterface>& interface) : backing(interface) {}
        virtual ~EmitterGenerator() = default;

        void Activate(const CpuContext& core, const std::shared_ptr<ReadableTextBlock>& textbl, u64 size);
        std::shared_ptr<EmitterThreadContext> GetThreadCtx();

        void WriteInstruction(const std::span<u8>& instruction);

        template <typename T> requires (std::is_integral_v<T>)
        void WriteInstruction(T instruction) {
            WriteInstruction(std::span(reinterpret_cast<u8*>(&instruction), sizeof(T)));
        }

        bool IsCompiled(const std::list<Ir::IrDescriptorFlowGraph>& is) const;
        virtual void Compile(const std::list<Ir::IrDescriptorFlowGraph>& list) = 0;

        u64 Execute() const;

        void ResetBuffer();

        boost::container::small_vector<u8, 16> lastInstruction{1}; // Assuming the first instruction is a single-byte NOP
        std::shared_ptr<EmitterInterface> backing;

        std::map<u64, std::shared_ptr<EmitterThreadContext>> cpus;

        virtual void EmitNop() = 0;
    private:
        void Advance(const std::shared_ptr<EmitterThreadContext>& context) const;
    };
}
