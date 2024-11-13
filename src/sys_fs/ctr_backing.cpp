#include <boost/align/align_down.hpp>
#include <boost/endian/conversion.hpp>
#include <sys_fs/ctr_backing.h>

namespace Plusnx::SysFs {
    constexpr auto CtrBlockSize{0x10};
    CtrBacking::CtrBacking(const FileBackingPtr& file, const Security::K128& key, const u64 offset, const u64 size, const std::array<u8, 16>& ctr) : FileBacking(file->path, file->mode), backing(file), file(offset, size), nonce(ctr) {
        switch (mode) {
            case FileMode::Write:
                encrypt.emplace(key.data(), key.size(), Security::OperationMode::CtrAes, false);
            case FileMode::Read:
                decrypt.emplace(key.data(), key.size(), Security::OperationMode::CtrAes, true);
            default: {}
        }
    }

    u64 CtrBacking::GetSize() const {
        if (mode == FileMode::Read && file.size)
            return file.size;
        return backing->GetSize();
    }

    u64 CtrBacking::ReadImpl(void* output, u64 size, u64 offset) {
        const auto padding{offset % CtrBlockSize};
        if (offset % CtrBlockSize) {
            size += padding;
            offset = boost::alignment::align_down(offset, CtrBlockSize);
        }
        UpdateNonce(offset + file.offset);

        if (size > puts.size())
            puts.resize(size);

        const auto result{backing->Read(puts.data(), size, offset + file.offset)};
        if (padding) {
            decrypt->Process(puts.data(), puts.data(), result);
            std::memcpy(output, &puts[padding], result - padding);
        } else {
            decrypt->Process(output, puts.data(), result);
        }

        return result - padding;
    }

    u64 CtrBacking::WriteImpl(const void* input, const u64 size, const u64 offset) {
        if (!encrypt || mode != FileMode::Write)
            throw Except("This CTR bank is not writable");
        assert(offset % 16 == 0);
        UpdateNonce(offset + file.offset);

        if (size > out.size())
            out.resize(size);

        encrypt->Process(out.data(), input, size);
        return backing->Write(out.data(), size, offset + file.offset);
    }

    void CtrBacking::UpdateNonce(const u64 offset) {
        u64 counter{offset >> 4};
        boost::endian::endian_reverse_inplace(counter);
        std::memcpy(&nonce[8], &counter, 8);

        decrypt->SetIvValue(nonce);
        if (encrypt)
            encrypt->SetIvValue(nonce);
    }
}
