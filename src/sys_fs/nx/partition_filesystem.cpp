#include <ranges>

#include <sys_fs/layered_fs.h>
#include <sys_fs/nx/partition_filesystem.h>
namespace Plusnx::SysFs::Nx {
    PartitionFilesystem::PartitionFilesystem(const FileBackingPtr& pfs) : RoDirectoryBacking("/" / pfs->path), backing(pfs) {
        if (pfs->Read(block) != sizeof(block)) {
            return;
        }
        if (block.magic == ConstMagic<u32>("HFS0"))
            hashable = true;
        if (!hashable && block.magic != ConstMagic<u32>("PFS0"))
            return;

        entrySize = hashable ? sizeof(HashableContentEntry) : sizeof(ContentEntry);
        const auto totalSize{entrySize * block.entries};
        const StringTable strings(totalSize + sizeof(block), block.strTableSize, pfs);

        content = pfs->GetBytes<u8>(totalSize, sizeof(block));
        dataOffset = strings.offset + strings.size;
        FileEntry entry{};
        for (u32 index{}; index < block.entries; index++) {
            assert(content.size() - entrySize * index >= sizeof(entry));
            std::memcpy(&entry, &content[entrySize * index], sizeof(entry));

            SysPath filename(strings.ReadString(entry.nameOffset));
            entries.emplace(std::move(filename), entry);
        }

        u64 total{};
        for (const auto& ensure : std::ranges::views::values(entries)) {
            total += ensure.size;
        }
        assert(pfs->GetSize() > total);

        const auto files{ListAllFiles()};
        if (!files.empty())
            first = files.front();
    }

    std::vector<SysPath> PartitionFilesystem::ListAllFiles() const {
        std::vector<SysPath> content;
        content.reserve(entries.size());
        for (const auto& filename : std::ranges::views::keys(entries)) {
            content.emplace_back(filename);
        }
        return content;
    }
    FileBackingPtr PartitionFilesystem::OpenFile(const SysPath& path, const FileMode mode) {
        assert(mode == FileMode::Read);
        if (!Exists(path))
            return {};
        return std::make_shared<FileLayered>(backing, path, dataOffset + entries[path].offset, entries[path].size);
    }

    StringTable::StringTable(const u32 offset, const u32 size, const FileBackingPtr& pfs) : offset(offset), size(size) {
        table = pfs->GetChars(size, offset);
    }
    std::string StringTable::ReadString(const u32 offset) const {
        if (offset > table.size())
            return {};

        std::string result;
        result.resize(std::strlen(&table[offset]));
        std::memcpy(result.data(), &table[offset], result.size());

        return result;
    }

    // https://switchbrew.org/wiki/ExeFS
    bool IsAExeFsPartition(const std::shared_ptr<PartitionFilesystem>& pfs) {
        return pfs->Exists("main") && pfs->Exists("main.npdm");
    }
}
