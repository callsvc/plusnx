#include <input/shared_hid_memory.h>

namespace Plusnx::Input {
    SharedHidBridge::SharedHidBridge(const std::shared_ptr<Core::Context>& context) : shrHid(std::make_unique<GenericKernel::Types::KSharedMemory>(*context->kernel)) {
    }
}
