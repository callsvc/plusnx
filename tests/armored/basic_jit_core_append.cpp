#include <array>

#include <armored/arm_types.h>
#include <armored/jit_context.h>
constexpr std::array<Plusnx::u64, 26> BasicA64Loop {
    0xD10043FF, // SUB     SP, SP, #0x10
    0xB9000BFF, // STR     WZR, [SP, #8]
    0xB9000FFF, // STR     WZR, [SP, #12]
    0x1400000B, // B       75C <MAIN+0x38>
    0xB9400FE1, // LDR     W1, [SP, #12]
    0x2A0103E0, // MOV     W0, W1
    0x531E7400, // LSL     W0, W0, #2
    0x0B010000, // ADD     W0, W0, W1
    0x531F7800, // LSL     W0, W0, #1
    0x1102F800, // ADD     W0, W0, #0xBE
    0xB9000BE0, // STR     W0, [SP, #8]
    0xB9400FE0, // LDR     W0, [SP, #12]
    0x11000400, // ADD     W0, W0, #0x1
    0xB9000FE0, // STR     W0, [SP, #12]
    0xB9400FE1, // LDR     W1, [SP, #12]
    0x5290D3E0, // MOV     W0, #0x869F // #34463
    0x72A00020, // MOVK    W0, #0x1, LSL #16
    0x6B00003F, // CMP     W1, W0
    0x54FFFE4D, // B.LE    734 <MAIN+0x10>
    0xB9400BE0, // LDR     W0, [SP, #8]
    0x910043FF, // ADD     SP, SP, #0x10
    0xD65F03C0, // RET
    0,
    0,
    0,
    0,
};


class Arm64v8BasedIoOverlay final : public Plusnx::Armored::JitContext {
public:
    Arm64v8BasedIoOverlay() : JitContext(Plusnx::Armored::GuestCpuType::Arm64) {
        memory.resize(sizeof(BasicA64Loop));
        std::memcpy(memory.data(), BasicA64Loop.data(), sizeof(BasicA64Loop));

        SetGuestMemory(memory);
    }

    void Svc(Plusnx::Armored::ArmRegistersContext& ctx) override {
        ctx.r0 = {};
        ctx.pc = {};
    }
    std::vector<Plusnx::u8> memory{};
};

int main() {
    Arm64v8BasedIoOverlay context;

    Plusnx::Armored::CpuContext cpu;
    context.AddCpu(cpu, Plusnx::Armored::AttachOp::AttachMainVma);

    assert(context.Run(std::size(BasicA64Loop)) > 8);

    const auto primary{context.GetCpu(0)};
    return primary->ctx.r0.W != 254;
}
