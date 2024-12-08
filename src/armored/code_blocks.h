#pragma once

#include <armored/arm_types.h>

namespace Plusnx::Armored {
    namespace Backend {
        class EmitterGenerator;
    }
    // Special class to allocate all pages required by the JIT
    enum class CodeBlockStatus {
        Success,
        MemoryMappingFailed
    };
    enum class CodeBlockProtectionStatus {
        Enable,
        Disable
    };

    class CodeBlocks {
    public:
        CodeBlocks(const std::shared_ptr<Backend::EmitterGenerator>& generator);
        void Initialize(u64 code);
        ~CodeBlocks();

        void Expand(u64 pages);
        void Protect(u8* begin, const u8* end, CodeBlockProtectionStatus action) const;

        u8* exec{nullptr};
        u64 size{};
        CodeBlockStatus status{};

    private:
        std::shared_ptr<Backend::EmitterGenerator> emitter;
    };
}
