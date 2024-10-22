#include <ranges>
#include <sys_fs/nx/partition_filesystem.h>

namespace Plusnx::SysFs::Nx {
    PartitionFilesystem::PartitionFilesystem(const FileBackingPtr& pfs) {
        pfs->Read(pfsSuper);
        bool shaFs{};
        if (pfsSuper.magic == ConstMagic<u32>("HFS0"))
            shaFs = true;

        const auto entrySize{shaFs ? sizeof(HashableContentEntry) : sizeof(ContentEntry)};
        const auto totalSize{entrySize * pfsSuper.entries};
        const StringTable strings(totalSize + sizeof(pfsSuper), pfsSuper.strTableSize, pfs);

        const auto content{pfs->GetBytes<u8>(totalSize, sizeof(pfsSuper))};
        FileEntry header{};
        for (u32 index{}; index < pfsSuper.entries; index++) {
            assert(content.size() - entrySize * index >= sizeof(header));
            std::memcpy(&header, &content[entrySize * index], sizeof(header));

            SysPath filename(strings.ReadString(header.nameOffset));
            entries.emplace(filename, header);
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
    FileBackingPtr PartitionFilesystem::OpenFile(const SysPath& path) {
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
