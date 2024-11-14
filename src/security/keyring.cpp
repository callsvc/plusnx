#include <ranges>
#include <boost/algorithm/string.hpp>

#include <core/context.h>
#include <security/keyring.h>
namespace Plusnx::Security {
    constexpr std::array keys256Names{"header_key", "sd_card_save_key_source", "sd_card_nca_key_source", "header_key_source", "sd_card_save_key"};
    constexpr std::array kekKeysPrefix{"titlekek_", "key_area_key_application_", "key_area_key_ocean_", "key_area_key_system_"};

    Key256Type GetKey256Alias(const std::string_view& key) {
        if (const auto result{std::ranges::find(keys256Names, key)}; result != std::end(keys256Names))
            return static_cast<Key256Type>(std::distance(keys256Names.begin(), result));
        return Key256Type::Invalid;
    }
    IndexedKeyType GetIndexKeyType(const std::string_view& key) {
        for (const auto [index, name] : std::ranges::views::enumerate(kekKeysPrefix)) {
            if (key.contains(name))
                return static_cast<IndexedKeyType>(index);
        }
        return IndexedKeyType::Invalid;
    }

    Keyring::Keyring(const std::shared_ptr<Core::Context> &context) {
        const auto provider{context->provider};

        for (const auto &name: keys256Names)
            keys256.emplace(GetKey256Alias(name), K256{});
        for (const auto &name: kekKeysPrefix)
            indexed.emplace(GetIndexKeyType(name), std::map<u32, K128>{});

        auto ReadKeys = [&](const std::string &keyPath, const KeyType type, const std::string &tag) {
            if (const auto keyFile{provider->OpenSystemFile(SysFs::RootId, keyPath)})
                ReadKeysPairs(keyFile, type);
            if ((type == KeyType::Production && prods.empty()) || (type == KeyType::Title && titles.empty())) {
                throw runtime_plusnx_except("{} key not found", tag);
            }
        };
        ReadKeys("prod.keys", KeyType::Production, "Production");
        ReadKeys("title.keys", KeyType::Title, "Title");
    }
    bool Keyring::AddTicket(std::unique_ptr<Ticket>&& ticket) {
        if (tickets.contains(ticket->GetRights().first))
            return {};

        AddTitlePair(ticket->GetRights().second, ticket->GetTitleKey());
        tickets.emplace(ticket->GetRights().first, std::move(ticket));
        return true;
    }

    bool Keyring::GetKey256(const Key256Type type, u8* output, const u64 size) const {
        std::memset(output, 0, size);
        if (keys256.contains(type)) {
            const auto& value{keys256.at(type)};
            std::memcpy(output, value.data(), size);
        }

        return !IsEmpty(std::span(output, size));
    }

    bool Keyring::GetIndexedKey(const IndexedKeyType type, const u32 index, u8 *output, const u64 size) const {
        if (indexed.contains(type) && indexed.at(type).contains(index)) {
            const auto &value{indexed.at(type).at(index)};
            std::memcpy(output, value.data(), size);
        }
        return !IsEmpty(std::span(output, size));
    }
    bool Keyring::GetTitleKey(const K128& key, u8* output, const u64 size) {
        if (const auto it{titles.find(key)}; it != titles.end()) {
            const auto& value{it->second};
            std::memcpy(output, value.data(), size);
        }
        return !IsEmpty(std::span(output, size));
    }

    void Keyring::ReadKeysPairs(const SysFs::FileBackingPtr& file, const KeyType type) {
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

        AddTitlePair(key, value);
    }
    void Keyring::AddTitlePair(const K128& key, const K128& value) {
        if (titles.contains(key))
            return;

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
