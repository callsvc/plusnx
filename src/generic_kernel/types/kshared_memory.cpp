#include <generic_kernel/types/kprocess.h>

#include <generic_kernel/types/kshared_memory.h>
namespace Plusnx::GenericKernel::Types {

    KSharedMemory::KSharedMemory(Kernel& kernel) : KAutoObject(kernel, Base::KAutoType::KSharedMemory) {
        const auto process{kernel.GetCurrentProcess()};
        ownerProcessId = process->pid;

        permissions.owner = MemoryProtection::Read;
        permissions.user = MemoryProtection::Read;
    }

    KSharedMemory::~KSharedMemory() {}

    // ReSharper disable once CppDFAConstantFunctionResult
    u8* KSharedMemory::Allocate([[maybe_unused]] std::span<u8> map, const std::shared_ptr<KProcess>& required) const {
        [[maybe_unused]] auto memoryPermissions{required->pid == ownerProcessId ? permissions.owner : permissions.user};
        return {};
    }
}
