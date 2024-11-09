#include <ranges>
#include <boost/algorithm/string.hpp>

#include <core/context.h>
#include <security/keyring.h>
namespace Plusnx::Security {
    constexpr std::array keys256Names{"header_key", "sd_card_save_key_source", "sd_card_nca_key_source", "header_key_source", "sd_card_save_key"};
    constexpr std::array kekKeysPrefix{"titlekek_", "key_area_key_application_", "key_area_key_ocean_", "key_area_key_system_"};

    Key256Type GetKey256Alias(const std::string_view& key) {
        if (key == "header_key")
            return Key256Type::HeaderKey;
        if (key == "sd_card_save_key_source")
            return Key256Type::SdSaveKeySave;
        if (key == "sd_card_nca_key_source")
            return Key256Type::NcaKeySource;
        if (key == "header_key_source")
            return Key256Type::KeySource;
        if (key == "sd_card_save_key")
            return Key256Type::SdSaveKey;
        return Key256Type::Invalid;
    }
    IndexedKeyType GetIndexKeyType(const std::string_view& key) {
        if (key.contains("titlekek_"))
            return IndexedKeyType::KekTitle;
        if (key.contains("key_area_key_application_"))
            return IndexedKeyType::KekAreaApplication;
        if (key.contains("key_area_key_ocean_"))
            return IndexedKeyType::KekAreaOcean;
        if (key.contains("key_area_key_system_"))
            return IndexedKeyType::KekAreaSystem;

        return IndexedKeyType::Invalid;
    }

    Keyring::Keyring(const std::shared_ptr<Core::Context>& context) {
        const auto provider{context->provider};

        for (const auto& name : keys256Names)
            keys256.emplace(GetKey256Alias(name), K256{});
        for (const auto& name : kekKeysPrefix)
            indexed.emplace(GetIndexKeyType(name), std::map<u32, K128>{});

        if (const auto prod{provider->OpenSystemFile(SysFs::RootId, "prod.keys")})
            ReadKeysPairs(prod, KeyType::Production);
        if (const auto title{provider->OpenSystemFile(SysFs::RootId, "title.keys")})
            ReadKeysPairs(title, KeyType::Title);

        if (prods.empty())
            throw Except("Production key not found");
        if (titles.empty())
            throw Except("Title key not found");
    }

    bool Keyring::GetKey256(const Key256Type type, u8* output, const u64 size) const {
        std::memset(output, 0, size);
        if (keys256.contains(type)) {
            const auto& value{keys256.at(type)};
            std::memcpy(output, value.data(), size);
        }

        return !IsEmpty(std::span(output, size));
    }

    bool Keyring::GetIndexedKey(const IndexedKeyType type, const u32 index, u8* output, const u64 size) const {
        std::memset(output, 0, size);
        if (indexed.contains(type) && indexed.at(type).contains(index)) {
            const auto& value{indexed.at(type).at(index)};
            std::memcpy(output, value.data(), size);
        }
        return !IsEmpty(std::span(output, size));
    }

    void Keyring::ReadKeysPairs(const SysFs::FileBackingPtr& file, [[maybe_unused]] const KeyType type) {
        assert(type == KeyType::Title ? titles.empty() : prods.empty());
        std::vector<std::string_view> pairs;

        const auto content{file->GetBytes<char>(file->GetSize())};

        split(pairs, std::string_view(content.data(), content.size()), boost::is_any_of("=\n"));
        if (pairs.size() % 2)
            pairs.pop_back();

        for (const auto& entries : std::views::chunk(pairs, 2)) {
            std::string key{entries[0]};
            std::string value{entries[1]};
            boost::trim(key);
            boost::trim(value);

            if (type == KeyType::Title)
                AddTitlePair(std::make_pair(key, value));
            else
                AddProductionPair(std::make_pair(key, value));
        }
    }

    void Keyring::AddTitlePair(const std::pair<std::string_view, std::string_view>& view) {
        const auto key{HexTextToByteArray<16>(view.first)};
        const auto value{HexTextToByteArray<16>(view.second)};

        titles.emplace(key, value);
    }
    void Keyring::AddProductionPair(const std::pair<std::string_view, std::string_view>& view) {
        const auto key{view.first};
        const auto value{view.second};

        if (const auto type = GetKey256Alias(key); type != Key256Type::Invalid) {
            const auto valueKey{HexTextToByteArray<32>(value)};

            if (keys256.contains(type))
                keys256[type] = valueKey;
        } else if (const auto type128 = GetIndexKeyType(key); type128 != IndexedKeyType::Invalid) {
            const auto valueKey{HexTextToByteArray<16>(value)};
            std::array<u8, 2> keyIndex;
            if (auto index = key.substr(key.find_last_of("_") + 1); std::ranges::all_of(index, isxdigit))
                keyIndex = HexTextToByteArray<2>(index);
            else
                return;

            u32 counter{};
            std::memcpy(&counter, keyIndex.data(), 2);

            const auto it{indexed.find(type128)};
            if (it != indexed.end()) {
                it->second.emplace(counter, valueKey);
            }
        } else {
            if (!prods.contains(key))
                prods.emplace(key, value);
        }
    }
}
