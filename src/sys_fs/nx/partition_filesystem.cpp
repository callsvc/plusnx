#include <ranges>

#include <sys_fs/layered_fs.h>
#include <sys_fs/nx/partition_filesystem.h>
namespace Plusnx::SysFs::Nx {
    PartitionFilesystem::PartitionFilesystem(const FileBackingPtr& pfs) : backing(pfs) {
        if (pfs->Read(superBlock) != sizeof(superBlock)) {
            return;
        }
        if (superBlock.magic == ConstMagic<u32>("HFS0"))
            hashable = true;

        entrySize = hashable ? sizeof(HashableContentEntry) : sizeof(ContentEntry);
        const auto totalSize{entrySize * superBlock.entries};
        const StringTable strings(totalSize + sizeof(superBlock), superBlock.strTableSize, pfs);

        content = pfs->GetBytes<u8>(totalSize, sizeof(superBlock));
        dataOffset = strings.offset + strings.size;
        FileEntry entry{};
        for (u32 index{}; index < superBlock.entries; index++) {
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
        if (!entries.contains(path))
            return {};
        return std::make_shared<FileLayered>(backing, path, dataOffset + entries[path].offset, entries[path].size);
    }

    StringTable::StringTable(const u32 offset, const u32 size, const FileBackingPtr& pfs) : offset(offset), size(size) {
        table = pfs->GetBytes<char>(size, offset);
    }
    std::string StringTable::ReadString(const u32 offset) const {
        if (offset > table.size())
            return {};

        std::string result;
        result.resize(std::strlen(&table[offset]));
        std::memcpy(result.data(), &table[offset], result.size());

        return result;
    }
}
