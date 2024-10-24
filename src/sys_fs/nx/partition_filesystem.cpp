#include <ranges>
#include <sys_fs/nx/partition_filesystem.h>

namespace Plusnx::SysFs::Nx {
    PartitionFilesystem::PartitionFilesystem(const FileBackingPtr& pfs) {
        if (pfs->Read(super) != sizeof(super)) {
            return;
        }
        if (super.magic == ConstMagic<u32>("HFS0"))
            hashable = true;

        entrySize = hashable ? sizeof(HashableContentEntry) : sizeof(ContentEntry);
        const auto totalSize{entrySize * super.entries};
        const StringTable strings(totalSize + sizeof(super), super.strTableSize, pfs);

        content = pfs->GetBytes<u8>(totalSize, sizeof(super));
        FileEntry header{};
        for (u32 index{}; index < super.entries; index++) {
            assert(content.size() - entrySize * index >= sizeof(header));
            std::memcpy(&header, &content[entrySize * index], sizeof(header));

            SysPath filename(strings.ReadString(header.nameOffset));
            entries.emplace(std::move(filename), header);
        }

        u64 total{};
        for (const auto& ensure : std::ranges::views::values(entries)) {
            total += ensure.size;
        }
        assert(pfs->GetSize() > total);
    }

    std::vector<SysPath> PartitionFilesystem::ListAllFiles() const {
        std::vector<SysPath> content(entries.size());
        for (const auto& filename : std::ranges::views::keys(entries)) {
            content.emplace_back(filename);
        }
        return content;
    }
    FileBackingPtr PartitionFilesystem::OpenFile(const SysPath& path) const {
        if (!entries.contains(path))
            return {};
        return {};
    }

    StringTable::StringTable(const u32 offset, const u32 size, const FileBackingPtr& pfs) {
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
