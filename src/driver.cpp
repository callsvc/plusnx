#include <iostream>

namespace Plusnx {
    bool IsWaylandPresent() {
        if (const auto session{getenv("XDG_SESSION_TYPE")})
            if (std::string_view(session) == "wayland")
                return true;

        const auto pipe{popen("loginctl show-session 2 -p Type", "r")};
        if (!pipe)
            return {};

        bool result{};
        std::array<char, 24> line;

        fgets(line.data(), sizeof(line), pipe);
        if (std::string_view(line).contains("wayland"))
            result = true;

        pclose(pipe);

        return result;
    }
}