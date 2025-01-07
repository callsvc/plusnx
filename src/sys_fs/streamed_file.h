#pragma once

#include <mutex>

#include <sys_fs/fs_types.h>
namespace Plusnx::SysFs {
    enum class CounterType {
        Read,
        Write
    };

    class StreamedFile final : public FileBacking {
    public:
        StreamedFile(FileBackingPtr&& file, u64 starts = 0, bool advise = {});

        u64 GetSize() const override;
        u64 SkipBytes(u64 count = 0, CounterType type = CounterType::Read);
        u64 GetCursor(CounterType type = CounterType::Read) const;

        u64 RemainBytes(CounterType type = CounterType::Read) const;

        FileBackingPtr _storage;
    private:
        u64 ReadImpl(void* output, u64 size, u64 offset) override;
        u64 WriteImpl(const void* input, u64 size, u64 offset) override;

        u64 rdOff, wrOff;
        std::mutex lock;
    };
}
