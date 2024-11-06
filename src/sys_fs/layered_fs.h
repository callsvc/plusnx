#pragma once

#include <sys_fs/fs_types.h>
namespace Plusnx::SysFs {
    class FileLayered final : public FileBacking {
    public:
        FileLayered(const FileBackingPtr& backing, const SysPath& name, u64 base, u64 bound);

        u64 GetSize() const override;
    private:
        u64 ReadImpl(void* output, u64 size, u64 offset) override;
        u64 WriteImpl(const void* input, u64 size, u64 offset) override;

        const FileBackingPtr parent;

        u64 starts;
        u64 ends;
    };
}