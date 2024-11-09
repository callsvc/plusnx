#pragma once
#include <armored/code_blocks.h>
#include <armored/cpu_context.h>
namespace Plusnx::Armored {
    enum class GuestCpuType {
        Arm32,
        Arm64
    };
    struct JitConfigs {
        u64 codeSectionSize{1024 * 4}; // Initial size of the independent code section
    };

    class JitContext {
    public:
        JitContext(GuestCpuType guest);
        void AddCpu(CpuContext& core);
        GuestCpuType type;
    private:
        std::shared_ptr<EmitterInterface> jitter;
        std::shared_ptr<EmitterDetails> details;

        JitConfigs configuration;
        std::map<u64, CodeBlocks> blocks;
    };
}
