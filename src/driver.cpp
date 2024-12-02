#include <format>
#include <iostream>

#include <SDL2/SDL.h>
#include <sys_fs/fsys/regular_file.h>
namespace Plusnx {
    std::string GetSdlVersion() {
        SDL_version version;
        SDL_GetVersion(&version);

        return std::format("{}.{}.{}", version.major, version.minor, version.patch);
    }
    bool IsWaylandPresent() {
        if (const auto session{getenv("XDG_SESSION_TYPE")})
            if (std::string_view(session) == "wayland")
                return true;

        i32 exists{};
        const SysFs::FSys::RegularFile waylandInfo("/usr/bin/wayland-info");
        if (waylandInfo)
            exists++;

        std::vector<char> loginOutput(100);
        if (const auto pipe{popen("loginctl show-session 2 -p Type", "r")}) {
            fgets(loginOutput.data(), loginOutput.size(), pipe);
            pclose(pipe);
        }
        if (std::string_view(loginOutput.data()).contains("wayland"))
            exists++;

        return exists > 0;
    }
}