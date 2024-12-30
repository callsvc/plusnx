#include <fstream>
#include <utility>

#include <fcntl.h>
#include <sys/stat.h>
#include <sys/sendfile.h>
#include <unistd.h>

#include <boost/align/align_up.hpp>
#include <boost/range/size.hpp>

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

            throw exception("Invalid file mode");
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

    constexpr auto MaximumFileDuplication{1024};
    RegularFile RegularFile::Duplicate(const SysPath& create) {
        const auto pathdup = [&] -> SysPath {
            if (!create.empty() && !exists(create))
                return create;
            for (u64 dupid{}; dupid < MaximumFileDuplication; dupid++) {
                if (const auto testpath{path / std::format("dup{}", dupid)}; !exists(testpath))
                    return testpath;
            }
            return {};
        }();

        std::optional<std::vector<u8>> buffer;

        const bool largefile{GetSize() > 1 * 1024 * 1024 * 1024};
        if (!largefile)
            buffer.emplace(4 * 1024 * 1024);

        RegularFile result{pathdup, FileMode::Write};
        if (!result)
            return {};

        for (u64 count{}; count < GetSize() && !largefile; ) {
            const auto read{Read(buffer->data(), buffer->size(), count)};
            if (!read)
                break;
            assert(result.Write(buffer->data(), read, count) == read);
            count += read;
        }
        u64 length{GetSize()}, ret{};
        const auto outdesc{result.descriptor};
        do {
            if (!largefile)
                break;
            // https://medium.com/swlh/linux-zero-copy-using-sendfile-75d2eb56b39b
            ret = sendfile(outdesc, descriptor, nullptr, length);
            length -= ret;
        } while (length > 0 && ret > 0);

        return result;
    }

    u64 RegularFile::ReadImpl(void* output, const u64 size, const u64 offset) {
        u64 falling{offset};
        u64 copied{};
#if !defined(_NDEBUG)
        std::memset(output, 0, size);
#endif
        errno = 0;

        const auto content{static_cast<u8*>(output)};
        static const std::vector<SysPath> SpecialInvalidSizeFiles{
            "/dev/urandom",
            "/proc/self/status",
            "/proc/self/maps",
            "/proc/sys/vm/max_map_count"
        };
        const auto realFile{!ContainsValue(SpecialInvalidSizeFiles, path)};

        static const auto pageSize{GetPageSize()};
        while (copied < size) {
            const u64 stride{size - copied > pageSize ? pageSize : size - copied};
            if (realFile && (!stride || copied + stride > GetSize() - offset))
                return stride;

            const auto result{pread64(descriptor, &content[copied], stride, falling)};

            copied += result;
            falling += result;

            if (static_cast<u64>(result) < stride) {
                if (result <= 0 && errno)
                    throw exception("Failed to read {} bytes from descriptor {}, error description: {}", size, descriptor, GetOsErrorString());
                break;
            }
        }
        return copied;
    }

    u64 RegularFile::WriteImpl(const void* input, const u64 size, const u64 offset) {
        Stat64 block;
        fstat64(descriptor, &block);

        const auto final{boost::alignment::align_up(offset + size, block.st_blksize)};
        if (GetSize() < final && mode != FileMode::Read && expandable) {
            if (fallocate64(descriptor, 0, offset, final) != 0)
                if (errno)
                    throw exception(GetOsErrorString());
        }

        const auto result{pwrite64(descriptor, input, size, offset)};
#if _POSIX_SYNCHRONIZED_IO > 0
        if (result)
            assert(fdatasync(descriptor) == 0);
#endif
        return result;
    }
}
