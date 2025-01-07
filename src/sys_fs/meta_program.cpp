#include <print>
#include <ranges>

#include <nxk/svc/svc_types.h>
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

        titleNpdm = NpdmHeader {
            .flags = NpdmHeader::Flags {
                .is64BitInstruction = true,
                .addressSpace = Nxk::AddressSpaceType::AddressSpace64Bit
            },
            .mainThreadPriority = ThreadPriority,
            .defaultCoreId = MainThreadCore,
            .systemResourceSize = SystemResourceSize,
            .mainThreadStackSize = MainThreadStack,
        };

        addressType = titleNpdm.flags.addressSpace;
        std::construct_at(&priorities, CreateThreadInfoRange(ThreadInfo >> 4));

    }
    static std::string_view GetSvcName(const u32 number) {
        static std::map<u32, std::string_view> svcList;
        static bool svcListInitialized{};
        if (!svcListInitialized) {
            svcList.clear();
            constexpr std::array callableCall{
                "",
                "SetHeapSize",
                "SetMemoryPermission",
                "SetMemoryAttribute",
                "MapMemory",
                "UnmapMemory",
                "QueryMemory",
                "ExitProcess",
                "CreateThread",
                "StartThread",
                "ExitThread",
                "SleepThread",
                "GetThreadPriority",
                "SetThreadPriority",
                "GetThreadCoreMask",
                "SetThreadCoreMask",
                "GetCurrentProcessorNumber",
                "SignalEvent",
                "ClearEvent",
                "MapSharedMemory",
                "UnmapSharedMemory",
                "CreateTransferMemory",
                "CloseHandle",
                "ResetSignal",
                "WaitSynchronization",
                "CancelSynchronization",
                "ArbitrateLock",
                "ArbitrateUnlock",
                "WaitProcessWideKeyAtomic",
                "SignalProcessWideKey",
                "GetSystemTick",
                "ConnectToNamedPort",
                "SendSyncRequestLight",
                "SendSyncRequest",
                "SendSyncRequestWithUserBuffer",
                "SendAsyncRequestWithUserBuffer",
                "GetProcessId",
                "GetThreadId",
                "Break",
                "OutputDebugString",
                "ReturnFromException",
                "GetInfo",
                "FlushEntireDataCache",
                "FlushDataCache",
                "MapPhysicalMemory",
                "UnmapPhysicalMemory",
                "GetFutureThreadInfo", // GetDebugFutureThreadInfo, 6.0.0+
                "GetLastThreadInfo",
                "GetResourceLimitLimitValue",
                "GetResourceLimitCurrentValue",
                "SetThreadActivity",
                "GetThreadContext3",
                "WaitForAddress",
                "SignalToAddress",
                "SynchronizePreemptionState",
                "GetResourceLimitPeakValue",
                "",
                "",
                "CreateIoPool",
                "CreateIoRegion",
                "",
                "KernelDebug", // [4.0.0+]
                "ChangeKernelTraceState",
                "",
                "CreateSession",
                "AcceptSession",
                "ReplyAndReceiveLight",
                "ReplyAndReceive",
                "ReplyAndReceiveWithUserBuffer",
                "CreateEvent",
                "MapIoRegion",
                "UnmapIoRegion",
                "MapPhysicalMemoryUnsafe",
                "UnmapPhysicalMemoryUnsafe",
                "SetUnsafeLimit",
                "CreateCodeMemory",
                "ControlCodeMemory",
                "SleepSystem",
                "ReadWriteRegister",
                "SetProcessActivity",
                "CreateSharedMemory",
                "MapTransferMemory",
                "UnmapTransferMemory",
                "CreateInterruptEvent",
                "QueryPhysicalAddress",
                "QueryMemoryMapping", // QueryIoMapping, 10.0.0+
                "CreateDeviceAddressSpace",
                "AttachDeviceAddressSpace",
                "DetachDeviceAddressSpace",
                "MapDeviceAddressSpaceByForce",
                "MapDeviceAddressSpaceAligned",
                "MapDeviceAddressSpace",
                "UnmapDeviceAddressSpace",
                "InvalidateProcessDataCache",
                "StoreProcessDataCache",
                "FlushProcessDataCache",
                "DebugActiveProcess",
                "BreakDebugProcess",
                "TerminateDebugProcess",
                "GetDebugEvent",
                "ContinueDebugEvent",
                "GetProcessList",
                "GetThreadList",
                "GetDebugThreadContext",
                "SetDebugThreadContext",
                "QueryDebugProcessMemory",
                "ReadDebugProcessMemory",
                "WriteDebugProcessMemory",
                "SetHardwareBreakPoint",
                "GetDebugThreadParam",
                "",
                "GetSystemInfo",
                "CreatePort",
                "ManageNamedPort",
                "ConnectToPort",
                "SetProcessMemoryPermission",
                "MapProcessMemory",
                "UnmapProcessMemory",
                "QueryProcessMemory",
                "MapProcessCodeMemory",
                "UnmapProcessCodeMemory",
                "CreateProcess",
                "StartProcess",
                "TerminateProcess",
                "GetProcessInfo",
                "CreateResourceLimit",
                "SetResourceLimitLimitValue",
                "CallSecureMonitor",
            };

            for (const auto& [index, callTitle] : std::views::enumerate(callableCall)) {
                if (std::string_view{callTitle}.empty())
                    continue;
                svcList.emplace(index, callTitle);
            }
            svcList.emplace(0x90, "MapInsecurePhysicalMemory"); // [15.0.0+]
            svcList.emplace(0x91, "UnmapInsecurePhysicalMemory");
            svcListInitialized = true;
        }

        if (svcList.contains(number))
            return svcList[number];
        return {};
    }

    constexpr std::array metaMagicValues{ConstMagic<u32>("META"), ConstMagic<u32>("ACID"), ConstMagic<u32>("ACI0")};

    MetaProgram::MetaProgram(FileBackingPtr&& npdm) :
        _metaSf(std::make_unique<StreamedFile>(std::move(npdm))) {

        if (_metaSf->Read(titleNpdm) != sizeof(titleNpdm))
            throw exception("Cannot read NPDM content");
        assert(titleNpdm.magic == metaMagicValues.front());

        title.emplace(titleNpdm.titleName.data());

        if (!title->contains("Application")) {
            std::print("Loading metadata from an unofficial application\n");
        }

        std::vector<u32> capabilities;
        auto CaptureAllKacValues = [&](const SectionAddr& kernel) {
            assert(!(kernel.size % 4));
            const auto acidKac{_metaSf->GetBytes<u32>(kernel.size / sizeof(u32), kernel.offset)};
            std::ranges::copy(acidKac, std::back_inserter(capabilities));
        };

        AcidHeader acid;
        if (_metaSf->Read(acid) == sizeof(acid)) {
            assert(acid.magic == metaMagicValues[1]);
            if (acid.sec[KernelCapability].size)
                CaptureAllKacValues(acid.sec[KernelCapability]);

            _metaSf->SkipBytes(titleNpdm.aci0.offset - _metaSf->GetCursor());
        }
        Aci0Header aci0;
        if (_metaSf->Read(aci0) == sizeof(aci0)) {
            assert(aci0.magic == metaMagicValues.back());

            if (aci0.sec[KernelCapability].size)
                CaptureAllKacValues(aci0.sec[KernelCapability]);

            titleId.emplace(aci0.programId);
        }

        SetKac(capabilities);

        // Due to the ease of raw patches in NPDM files, we must validate the read parameters before considering them
        addressType = titleNpdm.flags.addressSpace;
        assert(aci0.magic && titleId);

        assert(titleNpdm.systemResourceSize < 0x1FE00000);
    }

    void MetaProgram::DisplayBinaryInformation() const {
        std::print("Used address space: {} Bits\n", GetHostBitsCount(addressType));
        std::print("Main stack size: {:X}\n", titleNpdm.mainThreadStackSize);
        std::print("Thread priority range: {}\n", std::string(priorities.first));
        std::print("Default initial CPU ID: {}\n", titleNpdm.defaultCoreId);
        std::print("Title Name: {}\n", *title);

        u32 count{};
        std::println("System call in use:");
        for (const auto& [number, enable] : allowedSvc) {
            if (enable)
                count++;

            if (const auto syscall{GetSvcName(number)}; syscall.size())
                std::print("{}: {}\n", syscall, enable ? "Enabled" : "Disabled");
        }

        std::print("Count of allowed system calls: {}\n", count);
        std::print("Memory reserved for the system: {}\n", GetReadableSize(titleNpdm.systemResourceSize));
    }

    void MetaProgram::Populate(Nxk::Svc::CreateProcessParameter& creation) const {
        std::strncpy(creation.name.data(), titleNpdm.titleName.data(), creation.name.size());
        creation.category = Nxk::Svc::ProcessCategory::RegularTitle;

        if (const auto value = titleId)
            creation.titleId = *value;

        creation.systemResourceNumPages = titleNpdm.systemResourceSize / Nxk::SwitchPageSize;
        creation.is64BitInstruction = titleNpdm.flags.is64BitInstruction;
        creation.addressType = titleNpdm.flags.addressSpace;
        creation.isApplication = true;

        constexpr auto CodeStartOffset{0x500000UL};
        if (creation.is64BitInstruction) {
            creation.codeAddr = 0x8000000UL + CodeStartOffset;
        } else {
            creation.codeAddr = 0x200000UL + CodeStartOffset;
        }
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
#if 0 // It was difficult to find a title with syscall descriptors
        allowedSvc.emplace(0, true);
        allowedSvc.emplace(1, true);
        allowedSvc.emplace(2, false);
        allowedSvc.emplace(10, false);
        allowedSvc.emplace(11, false);
        allowedSvc.emplace(18, true);
#endif
    }

}
