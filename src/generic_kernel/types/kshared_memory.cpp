#include <generic_kernel/types/kprocess.h>

#include <generic_kernel/types/kshared_memory.h>
namespace Plusnx::GenericKernel::Types {

    KSharedMemory::KSharedMemory(Kernel& kernel) : KAutoObject(kernel, Base::KAutoType::KSharedMemory) {
        const auto process{kernel.GetCurrentProcess()};
        ownerProcessId = process->pid;

        permissions.owner = MemoryProtection::Read;
        permissions.user = MemoryProtection::Read;
    }

    u8* KSharedMemory::Allocate(u8* vaddr, const u64 size, const std::shared_ptr<KProcess>& process) const {
        const auto memoryPermissions{process->pid == ownerProcessId ? permissions.owner : permissions.user};
        auto* result{kernel.nxmemory->Allocate(vaddr, vaddr, size, memoryPermissions, MemoryType::Shared)};
        return result;
    }

    void KSharedMemory::Free(u8* vaddr, const u64 size, const std::shared_ptr<KProcess>& process) const {
        assert(process->pid);
        kernel.nxmemory->Free(vaddr, size);
    }
}
