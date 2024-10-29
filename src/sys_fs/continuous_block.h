#pragma once

#include <mutex>

#include <sys_fs/fs_types.h>
namespace Plusnx::SysFs {
    class ContinuousBlock final : public FileBacking {
    public:
        ContinuousBlock(const FileBackingPtr& file, const u64 starts = 0) : rcnt(starts), wcnt(starts), backing(file) {}

        u64 GetSize() const override;

    private:
        u64 ReadImpl(void* output, u64 size, u64 offset) override;
        u64 WriteImpl(const void* output, u64 size, u64 offset) override;

        u64 rcnt,
            wcnt;
        std::mutex lock;
        const FileBackingPtr& backing;
    };
}
