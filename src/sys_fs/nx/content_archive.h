#pragma once
#include <sys_fs/fs_types.h>

#include <security/cipher_cast.h>
#include <security/keyring.h>
namespace Plusnx::SysFs::Nx {
    enum class DistributionType : u8 {
        Download, // NSP types only
        GameCard // XCI types only
    };
    enum class ContentType : u8 {
        Program,
        Meta,
        Control,
        Manual,
        Data,
        PublicData
    };
    enum class KeyAreaIndex : u8 {
        Application,
        Ocean,
        System
    };

#pragma pack(push, 1)
    struct alignas(0x200) NcaHeader {
        std::array<u8, 0x100> firstHeaderSignature;
        std::array<u8, 0x100> secondHeaderSignature; // using a key from NPDM (or zeroes if not a program)
        u32 magic;
        DistributionType dist;
        ContentType type;
        u8 keyGenerationOld; // Must be equal to 2
        KeyAreaIndex areaIndex;
        u64 size;
        u64 programId;
        u32 contentIndex;
        u32 sdkAddonVersion;
        Security::KeyGeneration generation;
        u8 signatureGeneration;
        std::array<u8, 0xe> pad0;
        Security::RightsId rights;
    };
#pragma pack(pop)

    class NCA {
    public:
        NCA(const std::shared_ptr<Security::Keyring>& _keys, const FileBackingPtr& nca);

        static bool ValidateMagic(u32 magic);
        const FileBackingPtr backing;
    private:
        std::optional<Security::CipherCast> cipher;
        const std::shared_ptr<Security::Keyring>& keys;
        NcaHeader content;
        u64 counter{};
    };
}
