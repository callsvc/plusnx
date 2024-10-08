#pragma once
#include <filesystem>
#include <memory>

#include <sys_fs/provider.h>
namespace Plusnx {
    class Context;
}

namespace Plusnx::SysFs {
    class Assets {
        public:
        explicit Assets(const std::shared_ptr<Context>& context);
        void CheckDir(const std::filesystem::path& path) const;

        std::filesystem::path logs;
        std::filesystem::path games;
        std::filesystem::path keys;
        std::filesystem::path nand;
        std::filesystem::path cache;

        std::shared_ptr<Provider> provider;
    };
}