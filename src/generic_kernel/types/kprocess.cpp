#include <boost/align/align_up.hpp>
#include <generic_kernel/types/kprocess.h>

namespace Plusnx::GenericKernel::Types {
    KProcess::KProcess(Kernel& kernel) : KBaseType(kernel, KType::KProcess), ptb(kernel.userspace) {
    }

    void KProcess::Initialize() {
        pid = kernel.CreateProcessId();
    }
    void KProcess::Destroy() {
        pid = {};
    }

    void KProcess::SetProgramImage(const u64 baseAddr, std::array<std::span<u8>, 3> sections, const std::vector<u8>& program) const {
        ptb->MapProgramCode(ProgramCodeType::Text, baseAddr, std::span(sections[0]));

        const auto roOffset{boost::alignment::align_up(baseAddr + sections[0].size(), 4096)};
        ptb->MapProgramCode(ProgramCodeType::Ro, roOffset, std::span(sections[1]));

        auto deducedBssSize{program.size()};
        for (const auto& section : sections)
            deducedBssSize -= section.size();
        const auto dataOffset{boost::alignment::align_up(roOffset + sections[1].size(), 4096)};

        const std::span initialized{sections.back().data(), sections.back().size() + deducedBssSize};
        ptb->MapProgramCode(ProgramCodeType::Data, dataOffset, initialized);
    }
}
