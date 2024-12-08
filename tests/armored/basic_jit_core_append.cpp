#include <armored/arm_types.h>
#include <armored/jit_context.h>
int main() {
    Plusnx::Armored::JitContext context(Plusnx::Armored::GuestCpuType::Arm64);

    Plusnx::Armored::CpuContext cpu;
    context.AddCpu(cpu);
}
