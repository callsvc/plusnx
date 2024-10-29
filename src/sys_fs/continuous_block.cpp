#include <sys_fs/continuous_block.h>

namespace Plusnx::SysFs {
    u64 ContinuousBlock::GetSize() const {
        return backing->GetSize();
    }

    u64 ContinuousBlock::ReadImpl(void* output, const u64 size, const u64 offset) {
        std::lock_guard guard(lock);
        if (rcnt + offset > GetSize()) {
            throw std::runtime_error("Offset out");
        }
        const auto result{backing->Read(output, size, rcnt + offset)};

        rcnt += size;
        return result;
    }

    u64 ContinuousBlock::WriteImpl(const void* output, const u64 size, const u64 offset) {
        std::lock_guard guard(lock);
        const auto result{backing->Write(output, size, wcnt + offset)};
        wcnt += size;
        return result;
    }
}
