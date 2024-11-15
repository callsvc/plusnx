#pragma once

#include <security/key_types.h>
#include <security/ticket.h>
#include <sys_fs/provider.h>
namespace Plusnx::Core {
    class Context;
}
namespace Plusnx::Security {
    enum class Key256Type {
        HeaderKey,
        SdSaveKeySave,
        NcaKeySource,
        KeySource,
        SdSaveKey,
        Invalid
    };
    enum class IndexedKeyType {
        KekTitle,
        KekAreaApplication,
        KekAreaOcean,
        KekAreaSystem,
        Invalid
    };

    enum class KeyType {
        Production,
        Title
    };

    struct K128Hash {
        auto operator ()(const K128& key) const {
            std::array<u64, 2> result;
            std::memcpy(result.data(), &key, sizeof(key));
            return std::hash<u64>()(result[0]) ^ std::hash<u64>()(result[1]) << 1;
        }
    };
    Key256Type GetKey256Alias(const std::string_view& key);

    class Keyring {
    public:

        Keyring(const std::shared_ptr<Core::Context>& context);

        bool AddTicket(std::unique_ptr<Ticket>&& ticket);
        bool GetKey256(Key256Type type, u8* output, u64 size) const;
        bool GetIndexedKey(IndexedKeyType type, u32 index, u8* output, u64 size) const;
        bool GetTitleKey(const K128& key, u8* output, u64 size);

    private:
        void ReadKeysPairs(const SysFs::FileBackingPtr& file, KeyType type);
        void AddTitlePair(std::pair<std::string, std::string>&& keyPair);
        void AddTitlePair(const K128& key, const K128& value);
        void AddProductionPair(std::pair<std::string, std::string>&& keyPair);

        std::unordered_map<K128, K128, K128Hash> titles{};
        std::unordered_map<std::string, std::string> prods{};
        std::map<u64, std::unique_ptr<Ticket>> tickets;

        std::unordered_map<Key256Type, K256> keys256{};
        std::unordered_map<IndexedKeyType, std::map<u32, K128>> indexed;
    };
}
