#pragma once
#include <filesystem>
#include <vector>

#include <types.h>
namespace Plusnx::SysFs {
    using SysPath = std::filesystem::path;

    class FileBacking {
    public:
        virtual ~FileBacking() = default;

        template <typename T>
        void Read(T& object) {
            ReadImpl(reinterpret_cast<void*>(&object), sizeof(object));
        }

    protected:
        virtual void ReadImpl(void* output, u64 size) = 0;
    };
    class RoDirectoryBacking {
    public:
        RoDirectoryBacking() = default;

        explicit RoDirectoryBacking(const SysPath& dir) : path(dir) {}
        virtual ~RoDirectoryBacking() = default;

        operator std::filesystem::path() const {
            return path;
        }

        virtual std::vector<SysPath> ListAllFiles() const = 0;
    protected:
        SysPath path;
    };
}