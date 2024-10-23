#include <gsk/types/kprocess.h>
#include <gsk/kernel.h>
namespace Plusnx::Gsk {
    Kernel::Kernel() {
        for (u32 core{}; core < Cpu::TotalCoresCount; core++)
            cpuCores[core].Initialize();
    }

    Kernel::~Kernel() {
        for (u32 core{}; core < Cpu::TotalCoresCount; core++)
            cpuCores[core].Destroy();
    }

    std::shared_ptr<Types::KProcess> Kernel::CreateNewProcess() {
        listProc.push_back(std::make_shared<Types::KProcess>(*this));
        return listProc.back();
    }

    u64 Kernel::CreateProcessId() {
        return seed.pid++;
    }

}