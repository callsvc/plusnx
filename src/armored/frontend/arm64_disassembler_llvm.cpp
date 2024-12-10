#include <llvm-c/Disassembler.h>
#include <llvm-c/Target.h>
#include <llvm-c/Core.h>
#include <armored/frontend/mach_arm64_disas.h>

namespace Plusnx::Armored::Frontend {
    MachArm64Disas::MachArm64Disas() {
        u32 major, minor, path;
        LLVMGetVersion(&major, &minor, &path);

        LLVMInitializeAArch64TargetInfo();
        LLVMInitializeAArch64TargetMC();
        LLVMInitializeAArch64Disassembler();

        context = LLVMCreateDisasm("aarch64", nullptr, 0, nullptr, nullptr);
        LLVMSetDisasmOptions(LLVMDisasmContextRef{context}, LLVMDisassembler_Option_AsmPrinterVariant);
    }

    MachArm64Disas::~MachArm64Disas() {
        LLVMDisasmDispose(LLVMDisasmContextRef{context});
    }

    std::string MachArm64Disas::to_string(u32 code) {
        std::string result;

        std::span instruction{reinterpret_cast<u8*>(&code), sizeof(code)};
        result.resize_and_overwrite(0x40, [&](char* buffer, const u64 size) {
            const auto sizeUsed{LLVMDisasmInstruction(LLVMDisasmContextRef{context}, instruction.data(), instruction.size(), program, buffer, size)};
            program += sizeUsed;
            return !sizeUsed ? 0UZ : std::strlen(buffer);
        });

        return result;
    }
}
