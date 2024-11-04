#include <ranges>
#include <print>

#include <security/checksum.h>
#include <sys_fs/fsys/rigid_directory.h>

#include <loader/eshop_title.h>
namespace Plusnx::Loader {
    EShopTitle::EShopTitle(const SysFs::FileBackingPtr& nsp) :
        AppLoader(AppType::Nsp, ConstMagic<u32>("PFS0")),
        pfs(std::make_unique<SysFs::Nx::PartitionFilesystem>(nsp)) {

        if (!CheckHeader(nsp) || status != LoaderStatus::None)
            return;

        const auto files{pfs->ListAllFiles()};

        std::optional<SysFs::Nx::NCA> cnmt;

#if 1
        if (const auto damaged = ValidateAllFiles(files)) {
            std::print("The NSP file is apparently corrupted, Damaged file: {}\n", damaged->string());
            return;
        }
#endif

        for (const auto& file : files) {
            const auto type{GetEntryFormat(file)};
            if (type == ContainedFormat::Nca)
                contents.push_back(pfs->OpenFile(file));
            else if (type == ContainedFormat::Cnmt)
                cnmt.emplace(pfs->OpenFile(file));

            if (type != ContainedFormat::Ticket)
                continue;

            ImportTicket(file);
        }

        IndexNcaEntries(cnmt);
    }

    bool EShopTitle::ExtractFilesInto(const SysFs::SysPath& path) const {
        SysFs::FSys::RigidDirectory gameFs(path, true);
        return true;
    }

    void EShopTitle::Load(std::shared_ptr<Core::Context>& process) {
        assert(pfs->ListAllFiles().size());
    }
    void EShopTitle::ImportTicket(const SysFs::SysPath& filename) const {
        [[maybe_unused]] const auto ticket{pfs->OpenFile(filename)};
    }
    void EShopTitle::IndexNcaEntries([[maybe_unused]] const std::optional<SysFs::Nx::NCA>& metadata) const {
        assert(contents.size());
    }

    std::optional<SysFs::SysPath> EShopTitle::ValidateAllFiles(const std::vector<SysFs::SysPath>& files) const {
        Security::Checksum checksum;

        std::array<u8, 32> result;
        std::vector<u8> buffer(4 * 1024 * 1024);

        for (const auto& path : files) {
            auto filename{path};
            if (filename.extension() != ".nca")
                continue;
            while (filename.has_extension())
                filename = filename.replace_extension();

            auto expected{HexTextToByteArray<u8, 16>(filename.string())};
            if (IsEmpty(expected))
                continue;

            const SysFs::Nx::NCA nca(pfs->OpenFile(path));
            const auto stream{std::make_unique<SysFs::ContinuousBlock>(nca.backing)};

            while (auto remain{stream->RemainBytes()}) {
                if (remain > buffer.size())
                    remain = buffer.size();
                const auto size{stream->Read(buffer.data(), remain, 0)};
                checksum.Update(buffer.data(), size);
            }

            checksum.Finish(std::span(result));
            if (!IsEqual(std::span(result).subspan(0, 16), std::span(expected)))
                return path;
        }

        return {};
    }
}
