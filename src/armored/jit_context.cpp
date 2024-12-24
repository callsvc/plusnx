
#include <ranges>
#include <armored/backend/x86_64_emitter_context.h>
#include <armored/frontend/info_target_instructions.h>
#include <armored/frontend/arm64_translator.h>

#include <armored/jit_context.h>
namespace Plusnx::Armored {
    JitContext::JitContext(const GuestCpuType guest) {
        type = guest;
        if (type == GuestCpuType::Arm32) {
            throw exception("The JIT for ARM32 has not been implemented yet");
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

    JitContext::~JitContext() {
        // Removes redundancies and dependencies among shared objects
        if (jitter->backing)
            jitter->backing.reset();
        {
            [[maybe_unused]] auto&& _jitter{std::move(jitter)};
            _jitter->cpus.clear();
            blocks.clear();
        }
        assert(platform.use_count() == 1);
    }

    void JitContext::SetGuestMemory(const std::vector<u8>& vector) {
        assert(vector.size() >= 4);
        if (type == GuestCpuType::Arm64) {
            u32 first{};
            std::memcpy(&first, vector.data(), sizeof(u32));
            assert(Frontend::IsArm64Code(first));
        }
        vmainmap = std::span(vector);
    }

    void JitContext::AddCpu(CpuContext& core, const AttachOp attaching) {
        if (attaching == AttachOp::AttachMainVma) {
            core.vaddr64pointer = vmainmap;
        }

        if (blocks.contains(core.identifier)) {
            throw exception("The current context already exists");
        }

        if (const auto firstBlk{std::make_shared<ReadableTextBlock>(jitter)}) {
            jitter->Activate(core, firstBlk, jitParams.codeSectionSize);
            blocks.emplace(core.identifier, firstBlk);
        }
    }
    void JitContext::AddTicks(const u64 count) {
        ticks += count;
    }
    u64 JitContext::Run(const u64 count, const u64 id) {
        if (count)
            AddTicks(count);
        if (jitter->cpus.empty())
            return {};

        std::shared_ptr<Backend::EmitterThreadContext> thread{};
        for (const auto& context : std::ranges::views::values(jitter->cpus)) {
            if (id && context->cpuCtx->identifier == id)
                thread = context;
            if (const auto threadCtx{jitter->GetThreadCtx()}; threadCtx && !id)
                thread = context;
        }
        if (thread == nullptr)
            return {};

        const u64 result = [&] {
            platform->Translate(&thread->cpuCtx->vaddr64pointer[thread->cpuCtx->ctx.pc.X], count);

            if (!jitter->IsCompiled(platform->GetList())) {
                jitter->Compile(platform->GetList());
            }
            return jitter->Execute();
        }();

        return !result ? count : result;
    }

    std::optional<CpuContext> JitContext::GetCpu(const u64 index) const {
        if (jitter->cpus.size() < index)
            return {};
        for (const auto& [_index, cpuPair] : std::views::enumerate(jitter->cpus)) {
            if (static_cast<u64>(_index) == index)
                return cpuPair.second->cpuCtx;
        }
        return {};
    }
}
