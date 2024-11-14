#include <armored/jit_context.h>

#include <armored/arm64_emitter_context.h>
#include <armored/arm64_details.h>
namespace Plusnx::Armored {
    JitContext::JitContext(const GuestCpuType guest) {
        type = guest;
        if (type == GuestCpuType::Arm32) {
            throw runtime_plusnx_except("The JIT for ARM32 has not been implemented yet");
        }

        details = std::make_shared<Arm64Details>();
        jitter = std::make_shared<Arm64EmitterContext>(details);

        std::print("Host CPU assigned to JIT {}, CPU rank: {}\n", caps.brand, caps.GetCpuRank());
    }

    void JitContext::AddCpu(CpuContext& core) {
        if (blocks.contains(core.ccid))
            return;

        blocks.emplace(core.ccid, CodeBlocks(jitter, configuration.codeSectionSize));
    }
}
