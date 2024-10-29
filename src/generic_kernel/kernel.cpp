#include <generic_kernel/types/kprocess.h>
#include <generic_kernel/kernel.h>
namespace Plusnx::GenericKernel {
    Kernel::Kernel() {
        for (u32 core{}; core < Cpu::TotalCoresCount; core++)
            cpuCores[core].Initialize();

        memory = std::make_unique<UserSpace>();
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