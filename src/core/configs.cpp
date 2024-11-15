#include <string>
#include <iostream>

#include <boost/container/small_vector.hpp>
#include <boost/unordered/unordered_flat_map.hpp>
#include <toml.hpp>

#include <sys_fs/fsys/regular_file.h>
#include <core/configs.h>
namespace Plusnx::Core {
    // https://github.com/ToruNiina/toml11/blob/main/examples/boost_container/container.cpp#L10
    struct Toml11BoostImpl {
        using comment_type = toml::preserve_comments;

        using boolean_type  = bool;
        using integer_type  = std::int64_t;
        using floating_type = double;
        using string_type   = std::string;

        template<typename T>
        using array_type = boost::container::small_vector<T, 8>;
        template<typename K, typename T>
        using table_type = boost::unordered_flat_map<K, T>;

        static toml::result<integer_type, toml::error_info>
        parse_int(const std::string& str, const toml::source_location& src, const std::uint8_t base) {
            return toml::read_int<integer_type>(str, src, base);
        }
        static toml::result<floating_type, toml::error_info>
        parse_float(const std::string& str, const toml::source_location& src, const bool is_hex) {
            return toml::read_float<floating_type>(str, src, is_hex);
        }
    };

    template <typename T>
    void GetValue(const std::string& property, T& value, const T& relay, const toml::value& tree) {
        value = {};
        if constexpr (std::is_trivial_v<T>)
            value = find_or(tree, property, relay);
    }

    void Configs::Initialize(const SysFs::SysPath& path) {
        {
            SysFs::FSys::RegularFile create(path, SysFs::FileMode::Write);
        }

        const auto input{parse<Toml11BoostImpl>(path, toml::spec::v(1, 1, 0))};
        GetValue("enable-nce", ence, {}, input);
        GetValue("extract-control", excfs, true, input);

        {
            const auto values{toml::find_or<std::vector<i32>>(input, "affinity-mask", {})};
            if (!values.empty())
                std::ranges::copy(values, std::back_inserter(affinity));
        }
    }

    template <typename T>
    void AddValue(const std::string& property, const T& value, toml::table& values) {
        values.insert_or_assign(property, value);
    }
    void Configs::ExportConfigs(const SysFs::SysPath& output) const {
        toml::table table{};
        AddValue("enable-nce", ence, table);
        AddValue("extract-control", excfs, table);

        if (exists(output))
            std::filesystem::remove(output);

        auto config{SysFs::FSys::RegularFile(output, SysFs::FileMode::Write)};
        config.expandable = {};
        const toml::value values(table);

        const auto sanitized{format(values)};
        config.Write(sanitized.data(), sanitized.size());
    }
}
