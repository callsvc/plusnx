#include <cstring>
#include <cerrno>

#include <sys/signal.h>

#include <types.h>
namespace Plusnx {
    std::string GetOsErrorString() {
        std::string error;
        // https://linux.die.net/man/3/strerror_r
#if (_POSIX_C_SOURCE >= 200112L) && ! _GNU_SOURCE
        error.resize(1024);
        strerror_r(errno, error.data(), error.size());
#else
        std::array<char, 1024> message;
        const auto output{strerror_r(errno, message.data(), message.size())};
        if (output)
            error.resize(std::strlen(output));
        std::strncpy(error.data(), output, error.size());
#endif

        return error;
    }

    void ActivateTrap() {
#if __has_builtin(__builtin_debugtrap)
        __builtin_debugtrap();
#elif defined(__linux__)
        raise(SIGTRAP);
#else
        __builtin_trap();
#endif
    }
}