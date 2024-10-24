#pragma once

#include <sys_fs/fs_types.h>
namespace Plusnx::SysFs {
    class FileBounded final : public FileBacking {
    public:
        FileBounded(const FileBackingPtr& backing, u64 base, u64 bound);

        u64 GetSize() const override;
    private:
        u64 ReadImpl(void *output, u64 size, u64 offset) override;

        FileBackingPtr parent;

        u64 starts;
        u64 ends;
    };
}