#pragma once

#include <sqlite3.h>

#include <sys_fs/fs_types.h>
namespace Plusnx::Core {
    // https://discord.com/developers/docs/rich-presence/best-practices
    struct GameInformation {
        std::string title;
        std::string subtitle;
        std::string details;
        std::string state;

        std::vector<u8> art;
    };
    class ProcessQol {
    public:
        ProcessQol(const SysFs::SysPath& database);
        ~ProcessQol();

        void ChangeGame(const GameInformation& game);
        std::vector<std::string> GetPlayedSessions(u64 maxCount);
    private:
        sqlite3* db{nullptr};
        sqlite3_stmt* stmt{nullptr};
    };
}