
#include <armored/backend/x86_64_emitter_context.h>
#include <armored/frontend/info_target_instructions.h>
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
            platform->parent = jitter;
        }

        std::print("Host CPU assigned to JIT {}, CPU rank: {}\n", caps.brand, caps.GetCpuRank());
    }
    void JitContext::SetGuestMemory(const std::vector<u8>& vector) {
        assert(vector.size() >= 4);
        if (type == GuestCpuType::Arm64) {
            u32 first{};
            std::memcpy(&first, vector.data(), sizeof(u32));
            assert(Frontend::IsArm64Code(first));
        }
        vmap = std::span(vector);
    }

    void JitContext::AddCpu(CpuContext& core, const AttachOp attaching) {
        if (attaching == AttachOp::AttachMainVma) {
            core.vaddr64pointer = vmap;
        }

        if (blocks.contains(core.ccid)) {
            std::println("The current context already exists");
            return;
        }

        if (const auto firstBlk{std::make_shared<ReadableTextBlock>(jitter)}) {
            jitter->PushCpuContext(core, firstBlk, jitParams.codeSectionSize);
            blocks.emplace(core.ccid, firstBlk);
        }
    }

    void JitContext::AddTicks(const u64 count) {
        ticks = count;
    }

    u64 JitContext::Run(const u64 count, const u64 index) {
        if (count)
            AddTicks(count);
        if (jitter->cpus.size() < index)
            return {};
        const auto cpuIt{jitter->cpus.begin() + index};

        const u64 result = [&] {
            platform->Translate(&cpuIt->first.vaddr64pointer[cpuIt->first.ctx.pc.X], count);
            if (!jitter->IsCompiled(platform->GetList())) {
                jitter->Compile(platform->GetList());
            }
            return jitter->Execute();
        }();

        return !result ? count : result;
    }

    CpuContext JitContext::GetCpu(const u64 index) const {
        if (jitter->cpus.size() < index)
            return {};
        auto cpuIt{jitter->cpus.begin()};
        cpuIt += index;
        return cpuIt->first;
    }
}
