#include <fstream>
#include <utility>

#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>

#include <sys_fs/fsys/regular_file.h>
namespace Plusnx::SysFs::FSys {
    using Stat64 = struct stat64;

    RegularFile::RegularFile(const SysPath& path) : FileBacking(path) {
        if (!exists(path))
            if (auto stream{std::fstream(path, std::ios::in | std::ios::out)})
                stream.close();

        descriptor = open(path.c_str(), O_RDONLY);
        if (descriptor < 2)
            std::exchange(descriptor, 0);
    }

    RegularFile::~RegularFile() {
        if (descriptor)
            close(descriptor);
    }

    u64 RegularFile::GetSize() const {
        Stat64 details;
        fstat64(descriptor, &details);
        assert(details.st_blocks);
        return details.st_size;
    }

    void RegularFile::ReadImpl(void* output, const u64 size, const u64 offset) {
        u64 falling{offset};
        u64 copied{};
#if defined(_NDEBUG)
        std::memset(output, 0, size);
#endif

        const auto content{static_cast<u8*>(output)};
        while (copied < size) {
            const u64 stride{size - copied > 4096 ? 4096 : size - copied};
            if (!stride || copied + stride > GetSize() - offset) {
                return;
            }

            const auto result{pread64(descriptor, &content[copied], stride, falling)};
            if (static_cast<u64>(result) != stride) {
            }
            copied += result;
            falling += result;
        }

    }
}
