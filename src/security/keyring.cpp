#include <ranges>
#include <boost/algorithm/string.hpp>
#include <security/keyring.h>

#include <core/context.h>
namespace Plusnx::Security {
    constexpr std::array aliasKeys{"header_key", "sd_card_save_key_source", "sd_card_nca_key_source", "header_key_source", "sd_card_save_key"};
    IndexedKeyType GetKeyAlias(const std::string_view& key) {
        static std::unordered_map<std::string_view, IndexedKeyType> indexed256Alias;

        [[unlikely]] if (indexed256Alias.empty()) {
            u32 counter{static_cast<u32>(IndexedKeyType::HeaderKey)};
            for (const auto& alias : aliasKeys) {
                indexed256Alias.emplace(alias, static_cast<IndexedKeyType>(counter++));
            }
        }
        if (indexed256Alias.contains(key)) {
            return indexed256Alias.at(key);
        }
        return IndexedKeyType::Invalid;
    }

    Keyring::Keyring(const std::shared_ptr<Core::Context>& context) {
        const auto provider{context->provider};

        if (const auto prod{provider->OpenSystemFile(SysFs::RootId, "prod.keys")})
            ReadKeysPairs(prod, KeyType::Production);
        if (const auto title{provider->OpenSystemFile(SysFs::RootId, "title.keys")})
            ReadKeysPairs(title, KeyType::Title);

        if (prods.empty())
            throw Except("Production key not found");
        if (titles.empty())
            throw Except("Title key not found");
    }

    bool Keyring::GetIndexed(const IndexedKeyType type, u8* output, const u64 size) const {
        std::memset(output, 0, size);
        if (!indexed.contains(type)) {
            throw Except("Key index {} not found", static_cast<u32>(type));
        }
        const auto& value{indexed.at(type)};
        std::memcpy(output, value.data(), size);

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
        if (value.size() > 64)
            return;
        const auto valueKey{HexTextToByteArray<32>(value)};

        if (const auto type = GetKeyAlias(key); type != IndexedKeyType::Invalid) {
            if (!indexed.contains(type))
                indexed.emplace(type, valueKey);
        } else {
            prods.emplace(key, valueKey);
        }
    }
}
