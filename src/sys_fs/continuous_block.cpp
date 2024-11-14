#include <sys_fs/continuous_block.h>

namespace Plusnx::SysFs {
    u64 ContinuousBlock::GetSize() const {
        return backing->GetSize();
    }
    u64 ContinuousBlock::SkipBytes(const u64 count, const CounterType type) {
        if (type == CounterType::Read)
            return rcnt += count;
        if (type == CounterType::Write)
            return wcnt += count;

        throw runtime_plusnx_except("Invalid skip operation");
    }
    u64 ContinuousBlock::GetCursor(const CounterType type) const {
        return type == CounterType::Read ? rcnt : wcnt;
    }

    u64 ContinuousBlock::RemainBytes(const CounterType type) const {
        return GetSize() - GetCursor(type);
    }

    u64 ContinuousBlock::ReadImpl(void* output, const u64 size, const u64 offset) {
        std::lock_guard guard(lock);
        if (rcnt + offset > GetSize()) {
            throw runtime_plusnx_except("Offset out");
        }
        const auto result{backing->Read(output, size, rcnt + offset)};

        rcnt += result;
        return result;
    }
    u64 ContinuousBlock::WriteImpl(const void* input, const u64 size, const u64 offset) {
        std::lock_guard guard(lock);
        const auto result{backing->Write(input, size, wcnt + offset)};
        wcnt += size;
        return result;
    }
}
