
#include <armored/arm_x86_disas.h>
namespace Plusnx::Armored {
    ArmX86Disas::ArmX86Disas(const DisasFlavourType flavour, const bool half) : thumb(half), type(flavour) {
        i32 major, minor;
        cs_version(&major, &minor);

        cs_arch archType{};
        cs_mode mode{CS_MODE_ARM};
        if (type == DisasFlavourType::Arm64) {
            archType = CS_ARCH_ARM64;
        }
        if (type == DisasFlavourType::Arm32) {
            archType = CS_ARCH_ARM;
            if (thumb)
                mode = CS_MODE_THUMB;
        }
        if (type == DisasFlavourType::X86_64) {
            archType = CS_ARCH_X86;
            mode = CS_MODE_64;
        }

        assert(cs_open(archType, mode, &handle) == CS_ERR_OK);
        assert(cs_option(handle, CS_OPT_DETAIL, CS_OPT_OFF) == CS_ERR_OK);
        assert(cs_option(handle, CS_OPT_SYNTAX, CS_OPT_SYNTAX_INTEL) == CS_ERR_OK);
    }

    ArmX86Disas::~ArmX86Disas() {
        cs_close(&handle);
    }

    std::vector<std::string> ArmX86Disas::to_string(const std::span<u8>& code) {
        std::vector<std::string> result;

        cs_insn* stones{nullptr};
        const auto counter{cs_disasm(handle, code.data(), code.size(), relativePc, 0, &stones)};

        for (u64 inst{}; inst < counter; ++inst) {
            auto content{std::format("{:X}:\t{}\t\t{}", stones[inst].address, stones[inst].mnemonic, stones[inst].op_str)};
            std::ranges::transform(content, content.begin(), toupper);
            result.emplace_back(std::move(content));

            relativePc += stones[inst].size;
        }
        if (stones)
            cs_free(stones, counter);

        return result;
    }

    ArmX86Disas::operator bool() const {
        return cs_errno(handle) == CS_ERR_OK;
    }
}
