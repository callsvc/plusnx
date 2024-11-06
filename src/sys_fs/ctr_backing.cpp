#include <sys_fs/ctr_backing.h>

namespace Plusnx::SysFs {
    CtrBacking::CtrBacking(const FileBackingPtr& file, const Security::K128& key, const u64 offset) : FileBacking(file->path, file->mode), backing(file), adrOffset(offset) {
        switch (mode) {
            case FileMode::Write:
                encrypt.emplace(key.data(), key.size(), Security::OperationMode::CtrAes, true);
            case FileMode::Read:
                decrypt.emplace(key.data(), key.size(), Security::OperationMode::CtrAes, false);
            default: {}
        }
    }

    u64 CtrBacking::GetSize() const {
        return backing->GetSize();
    }

    u64 CtrBacking::ReadImpl(void* output, const u64 size, const u64 offset) {
        UpdateNonce(offset + adrOffset);

        if (size > puts.size())
            puts.resize(size);

        backing->Read(puts.data(), size, offset + adrOffset);
        decrypt->Process(output, puts.data(), size);
        return size;
    }

    u64 CtrBacking::WriteImpl(const void* input, const u64 size, const u64 offset) {
        if (!encrypt || mode != FileMode::Write)
            throw Except("This CTR bank is not writable");
        UpdateNonce(offset + adrOffset);

        if (size > out.size())
            out.resize(size);

        encrypt->Process(out.data(), input, size);
        return backing->Write(out.data(), size, offset + adrOffset);
    }

    void CtrBacking::UpdateNonce(const u64 offset) {
        const u64 counter{offset >> 4};

        decrypt->SetIvValue(Security::GetSwitchCounter(counter));
        if (encrypt)
            encrypt->SetIvValue(Security::GetSwitchCounter(counter));
    }
}
