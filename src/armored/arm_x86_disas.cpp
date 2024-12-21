
#include <llvm-c/Disassembler.h>
#include <llvm-c/Target.h>
#include <llvm-c/Core.h>

#include <armored/arm_x86_disas.h>
namespace Plusnx::Armored {
    ArmX86Disas::ArmX86Disas(const DisasFlavourType flavour) : type(flavour) {
        u32 major, minor, path;
        LLVMGetVersion(&major, &minor, &path);

        switch (type) {
            case DisasFlavourType::Arm64:
                [[fallthrough]];
            case DisasFlavourType::Arm32:
                LLVMInitializeAArch64TargetInfo();
                LLVMInitializeAArch64TargetMC();
                LLVMInitializeAArch64Disassembler();
                break;
            case DisasFlavourType::X86_64:
                LLVMInitializeX86TargetInfo();
                LLVMInitializeX86TargetMC();
                LLVMInitializeX86Disassembler();
            default: {}
        }

        context = LLVMCreateDisasm(type == DisasFlavourType::X86_64 ? "x86" : "aarch64", nullptr, 0, nullptr, nullptr);
        LLVMSetDisasmOptions(LLVMDisasmContextRef{context}, LLVMDisassembler_Option_AsmPrinterVariant);
    }

    ArmX86Disas::~ArmX86Disas() {
        LLVMDisasmDispose(LLVMDisasmContextRef{context});
    }

    std::string ArmX86Disas::to_string(const std::span<u8>& instruction) {
        std::string result;

        result.resize_and_overwrite(0x40, [&](char* buffer, const u64 size) {
            const auto sizeUsed{LLVMDisasmInstruction(LLVMDisasmContextRef{context}, instruction.data(), instruction.size(), pc64, buffer, size)};
            pc64 += sizeUsed;
            return !sizeUsed ? 0UZ : std::strlen(buffer);
        });

        return result;
    }
}
