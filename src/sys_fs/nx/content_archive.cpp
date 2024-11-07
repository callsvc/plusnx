#include <sys_fs/nx/content_archive.h>

namespace Plusnx::SysFs::Nx {
    constexpr auto XtsMode{Security::OperationMode::XtsAes};
    constexpr auto SectorSize{0x200};

    NCA::NCA(const std::shared_ptr<Security::Keyring>& _keys, const FileBackingPtr& nca) : keys(_keys) {
        if (nca->Read(content) != sizeof(content))
            return;
        if (!ValidateMagic(content.magic)) {
            const auto headerKey{keys->GetHeaderKey()};
            cipher.emplace(headerKey.data(), headerKey.size(), XtsMode, true);

            cipher->Decrypt(content, counter, SectorSize);
            if (!ValidateMagic(content.magic))
                throw Except("Corrupted or invalid NCA");

            assert(content.size == nca->GetSize());
        }

        assert(content.keyGenerationOld == 2);
    }

    bool NCA::ValidateMagic(const u32 magic) {
        static std::vector<u32> magics;
        if (magics.empty()) {
            magics.reserve(4);
            for (const auto& value : {"NCA3", "NCA2", "NCA1", "NCA0"}) {
                magics.emplace_back(ConstMagic<u32>(value));
            }
        }
        return ContainsValue(magics, magic);
    }
}
