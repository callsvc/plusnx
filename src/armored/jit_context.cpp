
#include <armored/backend/x86_64_emitter_context.h>
#include <armored/frontend/arm64_translator.h>

#include <armored/jit_context.h>
namespace Plusnx::Armored {
    JitContext::JitContext(const GuestCpuType guest) {
        type = guest;
        if (type == GuestCpuType::Arm32) {
            throw runtime_exception("The JIT for ARM32 has not been implemented yet");
        }

        if (type == GuestCpuType::Arm64) {
            platform = std::make_shared<Frontend::Arm64Translator>();
#if defined(__x86_64__)
            jitter = std::make_shared<Backend::X86_64_EmitterContext>(platform);
#endif
            platform->Initialize(jitter);
        }

        std::print("Host CPU assigned to JIT {}, CPU rank: {}\n", caps.brand, caps.GetCpuRank());
    }

    void JitContext::AddCpu(CpuContext& core) {
        if (blocks.contains(core.ccid))
            return;

        const auto cpuBlock(std::make_shared<CodeBlocks>(jitter));
        jitter->PushCpuContext(core, cpuBlock, jitParams.codeSectionSize);

        blocks.emplace(core.ccid, cpuBlock);
    }
}
