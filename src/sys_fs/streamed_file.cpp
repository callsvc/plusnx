#include <ranges>
#include <fcntl.h>

#include <sys_fs/fsys/rigid_directory.h>
#include <sys_fs/streamed_file.h>
namespace Plusnx::SysFs {
    StreamedFile::StreamedFile(FileBackingPtr&& file, const u64 starts, const bool advise) : FileBacking(file->path), _storage(std::move(file)), rdOff(starts), wrOff(starts) {
        const auto descriptor = [&] -> i32 {
            const FSys::RigidDirectory fds("/proc/self/fd");
            for (const auto& _opened : fds.ListAllFiles() | std::ranges::views::drop(3)) {
                auto _path{_opened};
                if (is_symlink(_path))
                    _path = read_symlink(_opened);
                if (_storage->path == _path)
                    return static_cast<i32>(std::strtoll(_opened.filename().c_str(), nullptr, 10));
            }
            return -1;
        }();
        if (descriptor != -1 && advise)
            assert(posix_fadvise(descriptor, starts, GetSize() - starts, POSIX_FADV_SEQUENTIAL) == 0);
    }

    u64 StreamedFile::GetSize() const {
        return _storage->GetSize();
    }
    u64 StreamedFile::SkipBytes(const u64 count, const CounterType type) {
        if (type == CounterType::Read)
            return rdOff += count;
        if (type == CounterType::Write)
            return wrOff += count;

        throw exception("Invalid skip operation");
    }
    u64 StreamedFile::GetCursor(const CounterType type) const {
        return type == CounterType::Read ? rdOff : wrOff;
    }

    u64 StreamedFile::RemainBytes(const CounterType type) const {
        return GetSize() - GetCursor(type);
    }

    u64 StreamedFile::ReadImpl(void* output, const u64 size, const u64 offset) {
        std::lock_guard guard(lock);
        if (offset)
            rdOff = offset;
        if (rdOff + size > GetSize()) {
            throw exception("Offset out");
        }
        const auto result{_storage->Read(output, size, rdOff)};

        rdOff += result;
        return result;
    }
    u64 StreamedFile::WriteImpl(const void* input, const u64 size, const u64 offset) {
        std::lock_guard guard(lock);
        if (offset)
            wrOff = offset;
        const u64 result{wrOff};
        if (const auto write{_storage->Write(input, size, wrOff)})
            wrOff += write;
        return wrOff - result;
    }
}
