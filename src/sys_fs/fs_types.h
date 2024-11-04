#pragma once
#include <filesystem>
#include <functional>
#include <map>

#include <types.h>

namespace Plusnx::SysFs {
    using SysPath = std::filesystem::path;
    enum class FileMode {
        Read,
        Write,
    };

    class FileBacking {
    public:
        FileBacking() = default;
        FileBacking(const SysPath& file, const FileMode type = FileMode::Read) : path(file), mode(type) {}
        virtual ~FileBacking() = default;

        template <typename T> requires (std::is_trivial_v<T>)
        T Read(const u32 offset = 0) {
            T object;
            ReadImpl(reinterpret_cast<void*>(&object), sizeof(object), offset);
            return object;
        }
        template <typename T>
        u64 Read(T& object, const u64 offset = 0) {
            return ReadImpl(reinterpret_cast<void*>(&object), sizeof(object), offset);
        }
        u64 Read(void* output, const u64 size, const u64 offset = 0) {
            return ReadImpl(output, size, offset);
        }
        u64 Write(const void* output, const u64 size, const u64 offset = 0) {
            return WriteImpl(output, size, offset);
        }

        template <typename T = char> requires std::is_trivial_v<T>
        std::vector<T> GetBytes(const u64 requested, const u64 offset = 0) {
            if (requested > GetSize())
                return {};
            std::vector<T> content(requested);
            ReadImpl(content.data(), content.size() * sizeof(T), offset);
            return content;
        }

        template <typename T = char> requires std::is_trivial_v<T>
        u64 PutBytes(const std::vector<T>& content, const u64 offset = 0) {
            return WriteImpl(content.data(), content.size() * sizeof(T), offset);
        }

        SysPath path;
        FileMode mode;
        virtual u64 GetSize() const = 0;
    protected:
        virtual u64 ReadImpl(void* output, u64 size, u64 offset = 0) = 0;
        virtual u64 WriteImpl(const void* output, u64 size, u64 offset = 0) = 0;
    };
    using FileBackingPtr = std::shared_ptr<FileBacking>;

    class RoDirectoryBacking {
    public:
        RoDirectoryBacking() = default;

        explicit RoDirectoryBacking(const SysPath& dir) : path(dir) {}
        virtual ~RoDirectoryBacking() = default;

        operator std::filesystem::path() const {
            return path;
        }

        virtual FileBackingPtr OpenFile(const SysPath& path) = 0;
        virtual std::vector<SysPath> ListAllFiles() const = 0;
        SysPath path;
    };

    using FileLists = std::map<SysPath, FileBackingPtr>;

    struct Directory {
        FileLists files;
        std::map<SysPath, Directory> subdirs;
    };

    using BaseDirCallback = std::function<bool(Directory&, const SysPath& path)>;

    class FileSystem : public RoDirectoryBacking {
    public:
        FileSystem() = default;

        virtual void ExtractAllFiles(const SysPath& output);
    protected:
        static bool WalkDirectories(Directory& directory, SysPath& iterator, const SysPath& target, BaseDirCallback&& callback);
        static bool WalkDirectories(Directory& directory, const SysPath& target, BaseDirCallback&& callback) {
            SysPath super{};
            return WalkDirectories(directory, super, target, std::move(callback));
        }
    };
}