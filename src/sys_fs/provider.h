#pragma once

#include <unordered_map>

#include <types.h>
namespace Plusnx::SysFs {
    using SysPath = std::filesystem::path;

    constexpr std::string rootPathId{"system"};
    class Provider {
    public:
        Provider();
        [[nodiscard]] SysPath GetRoot() const;

        void RegisterSystemPath(const SysPath& directory);
    private:
        std::unordered_map<std::string, SysPath> dirs;
        u64 opened{};
        u64 fileMax{};
    };
}
