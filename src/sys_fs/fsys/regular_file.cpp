#include <fstream>
#include <utility>

#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>
#include <boost/align/align_up.hpp>

#include <sys_fs/fsys/regular_file.h>
namespace Plusnx::SysFs::FSys {
    using Stat64 = struct stat64;

    RegularFile::RegularFile(const SysPath& path, const FileMode mode) : FileBacking(path, mode) {
        if (!exists(path)) {
            create_directories(path.parent_path());
            if (auto stream{std::fstream(path, std::ios::out | std::ios::trunc)}) {
                if (stream.is_open())
                    stream.close();
            }
        }
        auto parameter = [&] {
            if (mode == FileMode::Read)
                return O_RDONLY;
            if (mode == FileMode::Write)
                return O_RDWR;

            throw Except("Invalid file mode");
        }();
        descriptor = open(path.c_str(), parameter);
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
        assert(details.st_blksize);
        return details.st_size;
    }

    u64 RegularFile::ReadImpl(void* output, const u64 size, const u64 offset) {
        u64 falling{offset};
        u64 copied{};
#if defined(_NDEBUG)
        std::memset(output, 0, size);
#endif

        const auto content{static_cast<u8*>(output)};
        while (copied < size) {
            const u64 stride{size - copied > 4096 ? 4096 : size - copied};
            if (!stride || copied + stride > GetSize() - offset) {
                return stride;
            }

            const auto result{pread64(descriptor, &content[copied], stride, falling)};
            if (static_cast<u64>(result) != stride) {
            }
            copied += result;
            falling += result;
        }
        return copied;
    }

    u64 RegularFile::WriteImpl(const void* input, const u64 size, const u64 offset) {
        Stat64 block;
        fstat64(descriptor, &block);

        const auto final{boost::alignment::align_up(offset + size, block.st_blksize)};
        if (GetSize() < final) {
            if (fallocate64(descriptor, 0, offset, final) != 0)
                if (errno)
                    throw Except("{}", GetOsErrorString());
        }
        return pwrite64(descriptor, input, size, offset);
    }
}
