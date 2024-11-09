#pragma once

#include <security/cipher_cast.h>
#include <sys_fs/fs_types.h>

namespace Plusnx::SysFs {
    class CtrBacking final : public FileBacking {
    public:
        CtrBacking(const FileBackingPtr& file, const Security::K128& key, u64 offset, u64 size = 0);

        u64 GetSize() const override;
    private:
        u64 ReadImpl(void* output, u64 size, u64 offset) override;
        u64 WriteImpl(const void* input, u64 size, u64 offset) override;
        void UpdateNonce(u64 offset);

        const FileBackingPtr backing;
        u64 adrOffset;
        u64 adrSize;

        std::vector<u8> puts;
        std::vector<u8> out;

        std::optional<Security::CipherCast> decrypt;
        std::optional<Security::CipherCast> encrypt;
    };
}
