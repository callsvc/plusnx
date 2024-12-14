#include <armored/frontend/mach_arm64_disas.h>

namespace Plusnx::Armored::Frontend {
    MachArm64Disas::MachArm64Disas() {}
    MachArm64Disas::~MachArm64Disas() {}

    // ReSharper disable once CppMemberFunctionMayBeStatic
    std::string MachArm64Disas::to_string([[maybe_unused]] const u32 code) {
        return {};
    }
}
