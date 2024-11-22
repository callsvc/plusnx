#include <pwd.h>

#include <core/process_qol.h>
namespace Plusnx::Core {
    ProcessQol::ProcessQol(const SysFs::SysPath& database) {
        sqlite3_initialize();
        std::print("Loaded SQLite3 library version: {}\n", sqlite3_libversion());
        const std::string& pathname{database};
        constexpr auto traits{SQLITE_OPEN_READWRITE | SQLITE_OPEN_CREATE};
        if (sqlite3_open_v2(pathname.data(), &db, traits, nullptr) != SQLITE_OK)
            throw runtime_plusnx_except(sqlite3_errmsg(db));

        assert(sqlite3_prepare_v2(db, "CREATE TABLE IF NOT EXISTS Played(Id INTEGER PRIMARY KEY, SessionText TEXT)", -1, &stmt, nullptr) == SQLITE_OK);

        sqlite3_step(stmt);
        sqlite3_reset(stmt);
    }
    ProcessQol::~ProcessQol() {
        if (stmt)
            sqlite3_finalize(stmt);
        sqlite3_close_v2(db);

        sqlite3_shutdown();
    }

    std::string GetUserName() {
        const auto userid{getuid()};
        const auto pwd{getpwuid(userid)};

        std::string result(pwd->pw_gecos);
        return result;
    }

    void ProcessQol::ChangeGame(const GameInformation &game) {
        namespace chr = std::chrono;
        const auto current{std::format("{}", chr::floor<chr::seconds>(chr::system_clock::now()))};
        const auto command{std::format("INSERT INTO Played(SessionText) VALUES('{} started the game named {} at {}');)", GetUserName(), game.title, current)};

        assert(sqlite3_prepare_v2(db, command.data(), -1, &stmt, nullptr) == SQLITE_OK);
        sqlite3_step(stmt);

        sqlite3_reset(stmt);
    }
    std::vector<std::string> ProcessQol::GetPlayedSessions(u64 maxCount) {
        std::vector<std::string> result(maxCount / 2);

        assert(sqlite3_prepare_v2(db, "SELECT * FROM Played", -1, &stmt, nullptr) == SQLITE_OK);

        while (sqlite3_step(stmt) != SQLITE_DONE) {
            if (!maxCount)
                break;
            maxCount--;
            assert(sqlite3_column_count(stmt) == 2);
            const auto count{sqlite3_data_count(stmt)};
            for (i32 line{}; line < count; line++) {
                if (sqlite3_column_type(stmt, line) == SQLITE_TEXT)
                    result.emplace_back(reinterpret_cast<const char*>(sqlite3_column_text(stmt, line)));
            }
        }

        return result;
    }
}
