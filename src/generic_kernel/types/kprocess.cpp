#include <boost/align/align_up.hpp>
#include <boost/align/align_down.hpp>

#include <generic_kernel/types/kprocess.h>
namespace Plusnx::GenericKernel::Types {
    KProcess::KProcess(Kernel& kernel) :
    KSynchronizationObject(kernel, Base::KAutoType::KProcess), thisMm(kernel.memory) {
    }

    void KProcess::Initialize() {
        pid = kernel.CreateProcessId();
    }
    void KProcess::Destroy() {
        pid = {};
    }

    void KProcess::SetProgramImage(u64& baseAddr, std::array<std::span<u8>, 3> sections, std::vector<u8>&& program) const {
        thisMm->MapProgramCode(ProgramCodeType::Text, baseAddr, std::span(sections[0]));
        const auto roOffset{boost::alignment::align_up(baseAddr + sections[0].size(), 4096)};
        thisMm->MapProgramCode(ProgramCodeType::Ro, roOffset, std::span(sections[1]));

        {
            auto overBssSize{program.size()};
            for (const auto& section : sections) {
                overBssSize -= boost::alignment::align_up(section.size(), 4096);
            }

            const auto dataOffset{boost::alignment::align_up(roOffset + sections[1].size(), 4096)};

            if (const std::span initialized{sections.back().data(), boost::alignment::align_up(sections.back().size() + overBssSize, 4096)}; initialized.size()) {
                thisMm->MapProgramCode(ProgramCodeType::Data, dataOffset, initialized);
                baseAddr += dataOffset + initialized.size();
            }
        }
    }
}
