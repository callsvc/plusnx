#pragma once

#include <armored/armor_types.h>
#include <armored/emitter_interface.h>
namespace Plusnx::Armored {
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
        CodeBlocks(const std::shared_ptr<EmitterInterface>& arm, u64 count);
        ~CodeBlocks();

        void Expand(u64 holes);
        void Protect(u64 starts, u64 ends, CodeBlockProtectionStatus action) const;

        void* segment{};
        u64 size{};
        CodeBlockStatus status;
    private:
        std::shared_ptr<EmitterInterface> emitter;
    };
}
