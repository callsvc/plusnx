#include <array>

#include <armored/arm_types.h>
#include <armored/jit_context.h>

constexpr std::array<Plusnx::u64, 26> BasicA64Loop {
    0XD10043FF, // sub     sp, sp, #0x10
    0XB9000BFF, // str     wzr, [sp, #8]
    0XB9000FFF, // str     wzr, [sp, #12]
    0X1400000B, // b       75c <main+0x38>
    0XB9400FE1, // ldr     w1, [sp, #12]
    0X2A0103E0, // mov     w0, w1
    0X531E7400, // lsl     w0, w0, #2
    0X0B010000, // add     w0, w0, w1
    0X531F7800, // lsl     w0, w0, #1
    0X1102F800, // add     w0, w0, #0xbe
    0XB9000BE0, // str     w0, [sp, #8]
    0XB9400FE0, // ldr     w0, [sp, #12]
    0X11000400, // add     w0, w0, #0x1
    0XB9000FE0, // str     w0, [sp, #12]
    0XB9400FE1, // ldr     w1, [sp, #12]
    0X5290D3E0, // mov     w0, #0x869f // #34463
    0X72A00020, // movk    w0, #0x1, lsl #16
    0X6B00003F, // cmp     w1, w0
    0X54FFFE4D, // b.le    734 <main+0x10>
    0XB9400BE0, // ldr     w0, [sp, #8]
    0X910043FF, // add     sp, sp, #0x10
    0XD65F03C0, // ret
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

    assert(context.Run(std::size(BasicA64Loop) - 1) > 8);

    const auto primary{context.GetCpu(0)};
    return primary.ctx.r0.W != 254;
}
