#include <boost/align/align_up.hpp>
#include <boost/align/align_down.hpp>

#include <generic_kernel/types/kprocess.h>
#include <generic_kernel/base/k_recursive_lock.h>
namespace Plusnx::GenericKernel::Types {
    KProcess::KProcess(Kernel& kernel) : KSynchronizationObject(kernel, Base::KAutoType::KProcess), handles(kernel), mm(kernel.memory) {
    }

    void KProcess::Initialize() {
        pid = kernel.CreateProcessId();
    }

    void KProcess::Destroy() {
        for (const auto thread : threads) {
            if (const auto handle{handles.GetThread(thread)})
                handle->Stop();
        }

        partialTlsSlots.clear();
        fullTlsPages.clear();
        threads.clear();

        pid = {};
    }

    void KProcess::SetProgramImage(u64& vaddr, std::array<std::span<u8>, 3> sections, const std::vector<u8>& program, const bool allocate) const {
        auto MapCode = [&](const ProgramCodeType type, const u64 offset, const std::span<u8>& section) {
            if (!allocate)
                return;

            auto* codeAddr{kernel.memory->code.begin().base() + offset};
            auto* backAddr{kernel.nxmemory->backing->data() + offset};
            mm->MapProgramCode(type, codeAddr, backAddr, section);

            std::print("Amount of allocated data: {}\n", SysFs::GetReadableSize(mm->records.back().used));
        };

        MapCode(ProgramCodeType::Text, vaddr, std::span(sections[0]));
        const auto roOffset{boost::alignment::align_up(vaddr + sections[0].size(), 4096)};
        MapCode(ProgramCodeType::Ro, roOffset, std::span(sections[1]));

        {
            auto overBssSize{program.size()};
            for (const auto& section : sections) {
                overBssSize -= boost::alignment::align_up(section.size(), 4096);
            }

            const auto dataOffset{boost::alignment::align_up(roOffset + sections[1].size(), 4096)};

            if (const std::span initialized{sections.back().data(), boost::alignment::align_up(sections.back().size() + overBssSize, 4096)}; initialized.size()) {
                MapCode(ProgramCodeType::Data, dataOffset, initialized);
                vaddr = dataOffset + initialized.size();
            }
        }
    }

    void KProcess::AllocateTlsHeapRegion() {

        KScopedLock lock(kernel);
        // If we have any partially filled TLS
        if (!partialTlsSlots.empty()) {
            const auto& tlsSlot{partialTlsSlots.front()};
            exceptionTlsArea = tlsSlot->AllocateSlot();
            if (!tlsSlot->HasAvailableSlots()) {
                auto&& tls{std::move(partialTlsSlots.front())};
                fullTlsPages.emplace_back(std::move(tls));
            }
            return;
        }
        auto tls{std::make_unique<KTlsPage>(kernel)};
        exceptionTlsArea = tls->AllocateSlot();

        if (!tls->HasAvailableSlots())
            fullTlsPages.emplace_back(std::move(tls));
        else
            partialTlsSlots.emplace_back(std::move(tls));
    }

    void KProcess::CreateThread() {
        const auto stackSize{npdm.stackSize};

        u8* stack = [&] {
            auto* stackRegion{kernel.memory->stack.data()};
            auto* kernelRegion{kernel.nxmemory->backing->data()};

            kernel.nxmemory->Allocate(stackRegion, kernelRegion, MemoryProtection::Read | MemoryProtection::Write, stackSize, MemoryType::Stack);
            return stackRegion + stackSize;
        }();

        std::scoped_lock guard(threadsLock);
        auto* tls{static_cast<u8*>(exceptionTlsArea)};
        auto* tentry{static_cast<u8*>(entry)};

        u16 thrHandle{};
        if (const auto thread{handles.Create<KThread>(thrHandle)}) {
            thread->Initialize(tentry, tls, stack);
            threads.emplace_back(thrHandle);
        }

    }
}
