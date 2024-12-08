#pragma once
#include <armored/code_blocks.h>
#include <armored/cpu_context.h>

#include <armored/backend/emitter_generator.h>
#include <armored/backend/emitter_interface.h>

#include <cpu/features.h>
namespace Plusnx::Armored {
    enum class GuestCpuType {
        Arm32,
        Arm64
    };
    struct JitConfigs {
        u64 codeSectionSize{64 * 1024}; // Initial size of the independent code section
    };

    class JitContext {
    public:
        JitContext(GuestCpuType guest);
        void AddCpu(CpuContext& core);
        GuestCpuType type;
    private:
        std::shared_ptr<Backend::EmitterGenerator> jitter;
        std::shared_ptr<Backend::EmitterInterface> platform;
        Cpu::Features caps;

        JitConfigs jitParams;
        std::multimap<u64, std::shared_ptr<CodeBlocks>> blocks;
    };
}
