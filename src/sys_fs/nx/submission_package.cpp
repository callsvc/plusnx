#include <sys_fs/nx/content_metadata.h>
#include <sys_fs/nx/submission_package.h>

namespace Plusnx::SysFs::Nx {
    SubmissionPackage::SubmissionPackage(const std::shared_ptr<Security::Keyring>& _keys, const FileBackingPtr& nsp) :
        pfs(std::make_unique<PartitionFilesystem>(nsp)), keys(_keys) {

        const auto files{pfs->ListAllFiles()};
        for (const auto& file : files) {
            if (GetEntryFormat(file) == ContainedFormat::Ticket)
                ImportTicket(file);
        }

        for (const auto& file : files) {
            const auto type{GetEntryFormat(file)};
            if (type == ContainedFormat::Nca)
                contents.emplace_back(std::make_shared<NcaCore>(keys, pfs->OpenFile(file)));
            else if (type == ContainedFormat::Cnmt)
                if (const auto nca{std::make_unique<NcaCore>(keys, pfs->OpenFile(file))}; nca->type == ContentType::Meta)
                    cnmt = std::make_unique<PartitionFilesystem>(nca->pfsList.front());
        }

        if (!cnmt)
            return;
        assert(cnmt->ListAllFiles().size());
        const ContentMetadata meta{cnmt->OpenFile(*cnmt->first)};

        for (const auto& nextNca : GetAllNcas()) {
            indexedNca[NcaType{nextNca->type, meta.type}].emplace_back(nextNca);
        }
    }
    void SubmissionPackage::ImportTicket(const SysPath &filename) const {
        const auto tikFile{pfs->OpenFile(filename)};
        keys->AddTicket(std::make_unique<Security::Ticket>(tikFile));
    }

    std::vector<std::shared_ptr<NcaCore>> SubmissionPackage::GetAllNcas() const {
        std::vector<std::shared_ptr<NcaCore>> ncas;
        ncas.reserve(contents.size());

        for (const auto& _nca: contents) {
            ncas.emplace_back(_nca);
        }
        return ncas;
    }
    std::vector<std::shared_ptr<NcaCore>> SubmissionPackage::GetIndexedNcas(const ContentType type, const ContentMetaType metaType) const {
        std::vector<std::shared_ptr<NcaCore>> ncas;
        if (const auto it{indexedNca.find({type, metaType})}; it != indexedNca.end()) {
            const auto& content{it->second};
            ncas.reserve(content.size());
            for (const auto& _nca : content)
                ncas.emplace_back(_nca);
        }

        return ncas;
    }
}
