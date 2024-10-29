#include <generic_kernel/user_space.h>
namespace Plusnx::GenericKernel {
    void UserSpace::CreateProcessMemory(const AddressSpaceType type) {
        this->type = type;
    }

}