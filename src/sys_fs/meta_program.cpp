#include <print>

#include <sys_fs/meta_program.h>
namespace Plusnx::SysFs {
    std::pair<Range<u32>, Range<u32>> CreateThreadInfoRange(const u32 value) {
        const auto highestPrior{value & 0x3F};
        const auto lowestPrior{value >> 0x6 & 0x3F};
        const auto lowCore{value >> 0xC & 0xFF};
        const auto highCore{value >> 0x14 & 0xFF};

        return std::make_pair(Range(lowestPrior, highestPrior), Range(lowCore, highCore));
    }

    MetaProgram::MetaProgram() {
        constexpr auto ThreadPriority{0x2C};
        constexpr auto SystemResourceSize{0};
        constexpr auto MainThreadCore{0};
        constexpr auto MainThreadStack{0x100000};

        constexpr auto ThreadInfo{0x30043F7};

        content = NpdmHeader {
            .flags = NpdmHeader::Flags {
                .is64BitInstruction = true,
                .addressSpace = GenericKernel::AddressSpaceType::AddressSpace64Bit
            },
            .mainThreadPriority = ThreadPriority,
            .defaultCoreId = MainThreadCore,
            .systemResourceSize = SystemResourceSize,
            .mainThreadStackSize = MainThreadStack,
        };

        addressType = content.flags.addressSpace;
        std::construct_at(&priorities, CreateThreadInfoRange(ThreadInfo >> 4));

    }

    constexpr std::array metaMagics{
        ConstMagic<u32>("META"),
        ConstMagic<u32>("ACID"),
        ConstMagic<u32>("ACI0")
    };

    MetaProgram::MetaProgram(const FileBackingPtr& npdm) :
        metaFile(std::make_unique<StreamedFile>(npdm)) {

        if (metaFile->Read(content) != sizeof(content))
            throw runtime_exception("Cannot read NPDM content");
        assert(content.magic == metaMagics[0]);
        title.emplace(content.titleName.data());

        if (!title->contains("Application")) {
            std::print("Loading metadata from an unofficial application\n");
        }

        std::vector<u32> capabilities;
        auto CaptureAllKacValues = [&](const SectionAddr& kernel) {
            assert(!(kernel.size % 4));
            const auto acidKac{npdm->GetBytes<u32>(kernel.size / sizeof(u32), kernel.offset)};
            std::ranges::copy(acidKac, std::back_inserter(capabilities));
        };

        AcidHeader acid;
        if (metaFile->Read(acid) == sizeof(acid)) {
            assert(acid.magic == metaMagics[1]);
            if (acid.sec[KernelCapability].size)
                CaptureAllKacValues(acid.sec[KernelCapability]);

            metaFile->SkipBytes(content.aci0.offset - metaFile->GetCursor());
        }
        Aci0Header aci0;
        if (metaFile->Read(aci0) == sizeof(aci0)) {
            assert(aci0.magic == metaMagics[2]);

            if (aci0.sec[KernelCapability].size)
                CaptureAllKacValues(aci0.sec[KernelCapability]);

            titleId.emplace(aci0.programId);
        }

        SetKac(capabilities);

        // Due to the ease of raw patches in NPDM files, we must validate the read parameters before considering them
        addressType = content.flags.addressSpace;
        assert(aci0.magic && titleId);

        assert(content.systemResourceSize < 0x1FE00000);
    }

    void MetaProgram::DisplayBinaryInformation() const {
        std::print("Used address space: {} Bits\n", GetHostBitsCount(addressType));
        std::print("Main stack size: {:#x}\n", content.mainThreadStackSize);
        std::print("Thread priority range: {}\n", std::string(priorities.first));
        std::print("Default initial CPU ID: {}\n", content.defaultCoreId);
        std::print("Title Name: {}\n", *title);

        u32 count{};
        for (const auto& [number, enable] : allowedSvc) {
            if (enable)
                count++;
            if (svcList.contains(number) && enable)
                std::print("System call in use: {}\n", svcList.at(number));
        }

        std::print("Count of allowed system calls: {}\n", count);
        std::print("Memory reserved for the system: {}\n", GetReadableSize(content.systemResourceSize));
    }

    void MetaProgram::SetKac(const std::vector<u32>& descriptors) {
        for (auto descriptor : descriptors) {
            if (descriptor == 0xFFFFFFFF)
                return;
            const auto pattern{std::countr_one(descriptor)};
            descriptor >>= pattern + 1;
            u32 parameter{};

            switch (pattern) {
                case 4:
                    parameter = (descriptor >> 24) * 0x18;
                    for (u32 syscall{}; syscall < 0x18 && parameter + syscall < 0xC0; syscall++) {
                        allowedSvc.emplace(parameter + syscall, descriptor & 1);
                        descriptor >>= 1;
                    }
                    break;
                case 3:
                    std::construct_at(&priorities, CreateThreadInfoRange(descriptor));
                    break;
                default: {}
            }
        }
    }

    std::map<u32, std::string_view> MetaProgram::svcList{
        {0x01, "SetHeapSize"},
        {0x02, "SetMemoryPermission"}
    };
}
