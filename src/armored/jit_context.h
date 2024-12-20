#pragma once
#include <armored/readable_text_block.h>
#include <armored/cpu_context.h>

#include <armored/backend/emitter_generator.h>
#include <armored/backend/emitter_interface.h>

#include <cpu/features.h>
namespace Plusnx::Armored {
    enum class AttachOp {
        None,
        AttachMainVma
    };

    enum class GuestCpuType {
        Arm32,
        Arm64
    };
    struct JitConfigs {
        u64 codeSectionSize{64 * 1024}; // Initial size of the independent code section
    };

    class JitContext {
    public:
        virtual ~JitContext() = default;

        JitContext(GuestCpuType guest);
        void AddCpu(CpuContext& core, AttachOp attaching);

        void AddTicks(u64 count);
        u64 Run(u64 count = 0, u64 index = 0);
        CpuContext GetCpu(u64 index) const;

        GuestCpuType type;

        std::span<const u8> vmap{};
        u64 ticks{};

    protected:
        virtual void Svc(ArmRegistersContext& ctx) = 0;
        void SetGuestMemory(const std::vector<u8>& vector);

    private:
        std::shared_ptr<Backend::EmitterGenerator> jitter;
        std::shared_ptr<Backend::EmitterInterface> platform;
        Cpu::Features caps;

        JitConfigs jitParams;
        std::multimap<u64, std::shared_ptr<ReadableTextBlock>> blocks;
    };
}
