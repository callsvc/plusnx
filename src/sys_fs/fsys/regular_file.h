#pragma once
#include <sys_fs/fs_types.h>

#include <types.h>
namespace Plusnx::SysFs::FSys {
    class RegularFile final : public FileBacking {
    public:
        RegularFile() = default;
        RegularFile(const SysPath& path, FileMode mode = FileMode::Read);
        ~RegularFile() override;

        u64 GetSize() const override;
    private:
        u64 ReadImpl(void* output, u64 size, u64 offset) override;
        u64 WriteImpl(const void* input, u64 size, u64 offset) override;

        i32 descriptor{};
    };
}
