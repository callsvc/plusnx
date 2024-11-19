#pragma once
#include <list>
#include <sys_fs/fs_types.h>

#include <security/cipher_cast.h>
#include <security/checksum.h>
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

    enum class FsType : u8 {
        RomFs,
        PartitionFs,
    };
    enum class HashType : u8 {
        Auto,
        None,
        HierarchicalSha256Hash,
        HierarchicalIntegrityHash,
        AutoSha3, // [14.0.0+]
        HierarchicalSha3256Hash, // [14.0.0+]
        HierarchicalIntegritySha3Hash // [14.0.0+]
    };

    enum class EncryptionType : u8 {
        Auto,
        None,
        AesXts,
        AesCtr,
        AesCtrEx,
        AesCtrSkipLayerHash, // [14.0.0+]
        AesCtrExSkipLayerHash // [14.0.0+]
    };
#pragma pack(push, 1)
    struct FsEntry {
        u32 startOffset;
        u32 endOffset;
        u64 pad0;
    };

    struct HierarchicalSha256Data {
        std::array<u8, 0x20> tableHash; // SHA256 hash over the hash-table at section-start+0 with the below hash-table size
        u32 blockSize;
        u32 layerCount; // always 2
        struct LayerRegion {
            u64 offset;
            u64 size;
        };
        std::array<LayerRegion, 2> regions;
        std::array<LayerRegion, 3> pad0;
        std::array<u8, 0x80> pad1;

    };
    struct IntegrityMetaInfo {
        u32 magic;
        u32 version;
        u32 hashSize;
        u32 maxLayers;
        struct HierarchicalIntegrityVerificationLevelInformation {
            u64 logicalOffset;
            u64 hashDataSize;
            u32 blockSize;
            u32 pad0;
        };
        std::array<HierarchicalIntegrityVerificationLevelInformation, 6> levels;
        std::array<u8, 0x20> signatureSalt;
        std::array<u8, 0x20> hash;
        std::array<u8, 0x18> pad0;

        static_assert(sizeof(levels) == 0x90);
    };

    struct FsHeader {
        u16 magic;
        FsType type;
        HashType hashType;
        EncryptionType encryptionType;
        u8 metaDataHashType; // [14.0.0+]
        u16 pad0;
        union {
            std::array<u8, 0xF8> hashData;

            HierarchicalSha256Data hash256;
            IntegrityMetaInfo hashIntegrity;

            static_assert(sizeof(hash256) == sizeof(hashData));
            static_assert(sizeof(hashIntegrity) == sizeof(hashData));
        };
        std::array<u8, 0x40> patchInfo; // (only used with game updates RomFs)
        u32 generation;
        u32 secureValue;
        std::array<u8, 0x30> sparseInfo;
        std::array<u8, 0x28> compressionInfo;
        std::array<u8, 0x30> metadataHashDataInfo; // [14.0.0+]
        std::array<u8, 0x30> pad1;
    };

    struct NcaHeader {
        std::array<u8, 0x100> fixedSignature;
        std::array<u8, 0x100> npdmSignature; // using a key from NPDM (or zeroes if not a program)
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
        u8 fixedGeneration;
        std::array<u8, 0xE> pad0;
        Security::RightsId rights;
        std::array<FsEntry, 4> entries;
        std::array<std::array<u8, 0x20>, 4> headersSum;
        std::array<std::array<u8, 0x10>, 4> encryptedKeyArea;

        std::array<u8, 0x400 - 0x340> pad1;
        std::array<FsHeader, 4> headers;
    };

    static_assert(sizeof(NcaHeader) == 0xC00);
#pragma pack(pop)

    class NcaCore {
    public:
        NcaCore(const std::shared_ptr<Security::Keyring>& _keys, const FileBackingPtr& nca);

        static bool ValidateMagic(u32 magic);
        std::vector<FileBackingPtr> GetBackingFiles(bool partition) const;
        std::vector<std::pair<FsType, FileBackingPtr>> GetBackingFiles() const;
        bool VerifyNca(std::array<u8, 0x10>& expected, Security::Checksum& checksum, std::vector<u8>& buffer, u64& readSize) const;

        std::list<FileBackingPtr> romfsList;
        std::list<FileBackingPtr> pfsList;

        ContentType type;
        SysPath path;
        bool rights{};

    private:
        void CreateFilesystemEntries();
        void CreateBackingFile(const FsEntry& entry, const FsHeader& header);

        Security::K128 GetTitleKey() const;
        Security::K128 GetAreaKey(EncryptionType encType) const;
        u64 GetKeyRevision() const;

        std::optional<Security::CipherCast> cipher;
        const std::shared_ptr<Security::Keyring>& keys;
        const FileBackingPtr backing;

        NcaHeader content;
        u64 counter{};

        bool verified{};
        bool encrypted{};
    };
}
