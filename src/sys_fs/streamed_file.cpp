#include <sys_fs/streamed_file.h>

namespace Plusnx::SysFs {
    u64 StreamedFile::GetSize() const {
        return backing->GetSize();
    }
    u64 StreamedFile::SkipBytes(const u64 count, const CounterType type) {
        if (type == CounterType::Read)
            return rdPos += count;
        if (type == CounterType::Write)
            return wrPos += count;

        throw runtime_exception("Invalid skip operation");
    }
    u64 StreamedFile::GetCursor(const CounterType type) const {
        return type == CounterType::Read ? rdPos : wrPos;
    }

    u64 StreamedFile::RemainBytes(const CounterType type) const {
        return GetSize() - GetCursor(type);
    }

    u64 StreamedFile::ReadImpl(void* output, const u64 size, const u64 offset) {
        std::lock_guard guard(lock);
        if (rdPos + offset > GetSize()) {
            throw runtime_exception("Offset out");
        }
        const auto result{backing->Read(output, size, rdPos + offset)};

        rdPos += result;
        return result;
    }
    u64 StreamedFile::WriteImpl(const void* input, const u64 size, const u64 offset) {
        std::lock_guard guard(lock);
        const auto result{backing->Write(input, size, wrPos + offset)};
        wrPos += result;
        return result;
    }
}
