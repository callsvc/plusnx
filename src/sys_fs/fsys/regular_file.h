#pragma once
#include <sys_fs/fs_types.h>

#include <types.h>
namespace Plusnx::SysFs::FSys {
    class RegularFile final : public FileBacking {
    public:
        RegularFile() = default;
        RegularFile(const SysPath& path);
        ~RegularFile() override;

        u64 GetSize() const override;
    private:
        void ReadImpl(void* output, u64 size, u64 offset) override;

        i32 descriptor{};
    };
}
