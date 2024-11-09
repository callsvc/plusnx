#pragma once
#include <armored/emitter_details.h>
namespace Plusnx::Armored {
    class CodeBlocks;

    class EmitterInterface {
    public:
        EmitterInterface(const std::shared_ptr<EmitterDetails>& info) : details(info) {}
        virtual ~EmitterInterface() = default;

        void ChangeBlockScheme(CodeBlocks* segment = nullptr);
        virtual void EmitNop() = 0;
        void WriteI(const std::span<u8>& instruction);
        template <typename T>
        void WriteI(T instruction) {
            WriteI(std::span(reinterpret_cast<u8*>(&instruction), sizeof(T)));
        }
        void AdvancePc();

        u64 pc{};

        CodeBlocks* blocks{nullptr};
        std::shared_ptr<EmitterDetails> details;
    };
}
