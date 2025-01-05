#include <ranges>
#include <fcntl.h>

#include <sys_fs/fsys/rigid_directory.h>
#include <sys_fs/streamed_file.h>
namespace Plusnx::SysFs {
    StreamedFile::StreamedFile(const FileBackingPtr& file, const u64 starts, const bool advise) : FileBacking(file->path), rdPos(starts), wrPos(starts), backing(file) {
        const auto descriptor = [&] -> i32 {
            const FSys::RigidDirectory fds("/proc/self/fd");
            for (const auto& _opened : fds.ListAllFiles() | std::ranges::views::drop(3)) {
                auto _path{path};
                if (is_symlink(_opened))
                    _path = read_symlink(_opened);
                if (file->path == _path)
                    return static_cast<i32>(std::strtoll(_opened.filename().c_str(), nullptr, 10));
            }
            return -1;
        }();
        if (descriptor != -1 && advise)
            assert(posix_fadvise(descriptor, starts, GetSize() - starts, POSIX_FADV_SEQUENTIAL) == 0);
    }

    u64 StreamedFile::GetSize() const {
        return backing->GetSize();
    }
    u64 StreamedFile::SkipBytes(const u64 count, const CounterType type) {
        if (type == CounterType::Read)
            return rdPos += count;
        if (type == CounterType::Write)
            return wrPos += count;

        throw exception("Invalid skip operation");
    }
    u64 StreamedFile::GetCursor(const CounterType type) const {
        return type == CounterType::Read ? rdPos : wrPos;
    }

    u64 StreamedFile::RemainBytes(const CounterType type) const {
        return GetSize() - GetCursor(type);
    }

    u64 StreamedFile::ReadImpl(void* output, const u64 size, const u64 offset) {
        std::lock_guard guard(lock);
        if (rdPos + offset > GetSize()) {
            throw exception("Offset out");
        }
        const auto result{backing->Read(output, size, rdPos + offset)};

        rdPos += result;
        return result;
    }
    u64 StreamedFile::WriteImpl(const void* input, const u64 size, const u64 offset) {
        std::lock_guard guard(lock);
        const auto result{backing->Write(input, size, wrPos + offset)};
        wrPos += result;
        return result;
    }
}
