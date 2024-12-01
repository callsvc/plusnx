#include <armored/jit_context.h>

#include <armored/arm64_emitter_context.h>
#include <armored/arm64_details.h>
namespace Plusnx::Armored {
    JitContext::JitContext(const GuestCpuType guest) {
        type = guest;
        if (type == GuestCpuType::Arm32) {
            throw runtime_exception("The JIT for ARM32 has not been implemented yet");
        }

        details = std::make_shared<Arm64Details>();
        jitter = std::make_shared<Arm64EmitterContext>(details);

        std::print("Host CPU assigned to JIT {}, CPU rank: {}\n", caps.brand, caps.GetCpuRank());
    }

    void JitContext::AddCpu(CpuContext& core) {
        if (blocks.contains(core.ccid))
            return;

        const auto cpuBlock(std::make_shared<CodeBlocks>(jitter));
        jitter->ChangeBlockScheme(cpuBlock);
        cpuBlock->Initialize(configuration.codeSectionSize);

        blocks.emplace(core.ccid,  cpuBlock);
    }
}
