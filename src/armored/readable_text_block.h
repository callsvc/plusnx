#pragma once

#include <armored/arm_types.h>

namespace Plusnx::Armored {
    namespace Backend {
        class EmitterGenerator;
    }
    // Special class to allocate all pages required by the JIT
    enum class TextStatus {
        Success,
        MemoryMappingFailed
    };
    enum class TextProtectionStatus {
        Enable,
        Disable
    };

    class ReadableTextBlock {
    public:
        ReadableTextBlock(const std::shared_ptr<Backend::EmitterGenerator>& generator);
        void Initialize(u64 code);
        ~ReadableTextBlock();

        void Expand(u64 pages);
        void Protect(u8* begin, const u8* end, TextProtectionStatus action) const;

        u8* text{nullptr};
        u64 size{};
        TextStatus status{};

    private:
        std::shared_ptr<Backend::EmitterGenerator> emitter;
    };
}
