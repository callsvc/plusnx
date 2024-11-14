#include <sys_fs/fs_types.h>

#include <sys_fs/fsys/regular_file.h>

namespace Plusnx::SysFs {
    bool FileSystem::WalkDirectories(Directory& directory, SysPath& iterator, const SysPath& target, BaseDirCallback&& callback) {
        for (auto& [path, dir]: directory.subdirs) {
            iterator /= path;
            if (WalkDirectories(dir, iterator, target, std::move(callback)))
                return true;
            if (callback(dir, iterator))
                return true;
            iterator = iterator.parent_path();
        }
        return callback(directory, iterator);
    }

    std::string GetReadableSize(const u64 amount) {
        constexpr std::array formats{
            "B",
            "KiB",
            "MiB",
            "GiB"
        };
        u32 fmt{};
        auto size{static_cast<double>(amount)};
        for (; size > 0x400; size /= 0x400)
            fmt++;
        if (fmt > std::size(formats))
            return {};

        return std::format("{:3.04f} {}", size, formats[fmt]);
    }
    void FileSystem::ExtractAllFiles(const SysPath& output) {
        const auto cachedFiles{ListAllFiles()};
        std::print("Extracting the contents of FS {}\n", path.string());
        for (const auto& filename : cachedFiles) {
            const auto file{OpenFile(filename)};
            const auto outputFile(std::make_unique<FSys::RegularFile>(output / filename.relative_path(), FileMode::Write));
            std::print("Extracting {} to path {}\n", file->path.string(), outputFile->path.string());

            auto content(file->GetBytes(file->GetSize()));
            outputFile->PutBytes(content);
        }
    }
}
