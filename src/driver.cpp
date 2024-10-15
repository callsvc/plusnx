#include <iostream>

namespace Plusnx {
    bool IsWaylandPresent() {
        if (const auto session{getenv("XDG_SESSION_TYPE")})
            if (std::string_view(session) == "wayland")
                return true;

        std::array<char, 24> line{};
        {
            const auto pipe{popen("loginctl show-session 2 -p Type", "r")};
            if (!pipe)
                return {};
            fgets(line.data(), sizeof(line), pipe);

            pclose(pipe);
        }
        return std::string_view(line).contains("wayland");
    }
}