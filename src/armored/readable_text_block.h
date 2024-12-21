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

    class ReadableTextBlock : public std::enable_shared_from_this<ReadableTextBlock> {
    public:
        ReadableTextBlock(const std::shared_ptr<Backend::EmitterGenerator>& _emitter);
        void Initialize(u64 code);
        ~ReadableTextBlock();

        void Expand(u64 expansion = 0);
        void Protect(u8* begin, const u8* end, TextProtectionStatus action) const;
        void Protect(TextProtectionStatus prot) const;

        u8* text{nullptr};
        u64 size{};
        u64 seek{};
        TextStatus status{};

    private:
        std::shared_ptr<Backend::EmitterGenerator> emitter;
    };
}
