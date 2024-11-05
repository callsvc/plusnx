#pragma once

#include <security/key_types.h>
#include <sys_fs/provider.h>

namespace Plusnx::Core {
    class Context;
}
namespace Plusnx::Security {
    enum class IndexedKeyType {
        Invalid,
        HeaderKey,
        SdSaveKeySave,
        NcaKeySource,
        KeySource,
        SdSaveKey
    };
    enum class KeyType {
        Production,
        Title
    };

    IndexedKeyType GetKeyAlias(const std::string_view& key);

    struct K128Hash {
        auto operator ()(const K128& key) const {
            std::array<u64, 2> result;
            std::memcpy(result.data(), &key, sizeof(key));
            return std::hash<u64>()(result[0]) ^ std::hash<u64>()(result[1]);
        }
    };

    class Keyring {
    public:

        Keyring(const std::shared_ptr<Core::Context>& context);
        bool GetIndexed(IndexedKeyType type, u8* output, u64 size) const;
        K256 GetHeaderKey() const {
            K256 result;
            GetIndexed(IndexedKeyType::HeaderKey, result.data(), result.size());
            return result;
        }

    private:
        void ReadKeysPairs(const SysFs::FileBackingPtr& file, KeyType type);
        void AddTitlePair(const std::pair<std::string_view, std::string_view>& view);
        void AddProductionPair(const std::pair<std::string_view, std::string_view>& view);

        std::unordered_map<K128, K128, K128Hash> titles{};
        std::unordered_map<std::string_view, K256> prods{};
        std::unordered_map<IndexedKeyType, K256> indexed{};
    };
}
