#include <sys_fs/fsys/regular_file.h>
#include <sys_fs/fsys/rigid_directory.h>
#include <sys_fs/streamed_file.h>

#include <sys_fs/fs_types.h>
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
        constexpr std::array formats{"B", "KiB", "MiB", "GiB"};
        u64 format{};

        auto size{static_cast<double>(amount)};
        for (; size > 0x400; size /= 0x400)
            format++;
        if (format > std::size(formats))
            return {};

        return std::format("{:.4} {}", size, formats[format]);
    }

    void RoDirectoryBacking::ExtractAllFiles(const SysPath &output) {
        const auto cachedFiles{ListAllFiles()};
        std::vector<u8> buffering(8 * 1024 * 1024);

        std::print("Extracting the contents of FS {}\n", GetPath());
        for (auto filename: cachedFiles) {
            const auto file{std::make_unique<StreamedFile>(OpenFile(filename))};
            const auto directory{std::make_unique<FSys::RigidDirectory>(output)};

            if (filename.has_relative_path())
                filename = filename.relative_path();

            if (directory->Exists(filename, true))
                return;
            const auto outputFile{std::make_unique<StreamedFile>(directory->CreateFile(filename))};
            std::print("Extracting {} to path {}\n", file->GetPath(), outputFile->GetPath());

            u64 stride{buffering.size()};
            while (file->RemainBytes()) {
                if (file->RemainBytes() < buffering.size())
                    stride = file->RemainBytes();
                file->Read(buffering.data(), stride);

                outputFile->Write(buffering.data(), stride);
            }
        }
    }
    bool RoDirectoryBacking::Exists(const SysPath& path, const bool relative) const {
        if (!relative)
            return ContainsValue(ListAllFiles(), path);

        for (const auto& filepath : ListAllFiles()) {
            const auto filestr{filepath.string()};
            const auto pathstr{path.string()};

            if (filestr.contains(pathstr))
                return true;
        }
        return {};
    }
}
