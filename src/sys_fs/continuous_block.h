#pragma once

#include <mutex>

#include <sys_fs/fs_types.h>
namespace Plusnx::SysFs {
    enum class CounterType {
        Read,
        Write
    };

    class ContinuousBlock final : public FileBacking {
    public:
        ContinuousBlock(const FileBackingPtr& file, const u64 starts = 0) : FileBacking(file->path), rcnt(starts), wcnt(starts), backing(file) {}

        u64 GetSize() const override;
        u64 SkipBytes(u64 count = 0, CounterType type = CounterType::Read);
        u64 GetCursor(CounterType type = CounterType::Read) const;

        u64 RemainBytes(CounterType type = CounterType::Read) const;
    private:
        u64 ReadImpl(void* output, u64 size, u64 offset) override;
        u64 WriteImpl(const void* input, u64 size, u64 offset) override;

        u64 rcnt,
            wcnt;
        std::mutex lock;
        const FileBackingPtr backing;
    };
}
