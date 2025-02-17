#include <nxk/types/kprocess.h>

#include <nxk/types/kshared_memory.h>
namespace Plusnx::Nxk::Types {

    KSharedMemory::KSharedMemory(Kernel& kernel) : KAutoObject(kernel, Base::KAutoType::KSharedMemory) {
        const auto process{kernel.GetCurrentProcess()};
        ownerProcessId = process->pid;

        permissions.owner = MemoryProtection::Read;
        permissions.user = MemoryProtection::Read;
    }

    u8* KSharedMemory::Allocate(u8* vaddr, const u64 size, const std::shared_ptr<KProcess>& process) const {
        const auto memoryPermissions{process->pid == ownerProcessId ? permissions.owner : permissions.user};
        auto* result{kernel.nxmemory->Allocate(vaddr, vaddr, size, MemoryType::Shared)};
        if (result)
            kernel.nxmemory->Protect(vaddr, size, memoryPermissions);
        return result;
    }

    void KSharedMemory::Free(u8* vaddr, const u64 size, const std::shared_ptr<KProcess>& process) const {
        assert(process->pid);
        kernel.nxmemory->Free(vaddr, size);
    }
}
