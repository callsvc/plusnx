#pragma once

#include <unordered_map>

#include <types.h>
namespace Plusnx::SysFs {
    constexpr std::string rootPathId{"system"};
    class Provider {
    public:
        Provider();
        [[nodiscard]] std::filesystem::path GetRoot() const;

        void RegisterSystemPath(const std::filesystem::path& directory);
    private:
        std::unordered_map<std::string, std::filesystem::path> dirs;
        u64 opened{};
        u64 fileMax{};
    };
}
