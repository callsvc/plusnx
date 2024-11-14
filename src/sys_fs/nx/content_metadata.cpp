#include <sys_fs/nx/content_metadata.h>

namespace Plusnx::SysFs::Nx {
    ContainedFormat GetEntryFormat(const SysPath& pathname) {
        const auto& extension{pathname.extension()};
        if (extension == ".nca") {
            if (pathname.has_filename())
                if (pathname.stem().extension() == ".cnmt")
                    return ContainedFormat::Cnmt;
            return ContainedFormat::Nca;
        }
        if (extension == ".tik")
            return ContainedFormat::Ticket;
        if (extension == ".xml")
            return ContainedFormat::Xml;

        return ContainedFormat::Invalid;
    }

    ContentMetadata::ContentMetadata(const FileBackingPtr& cnmt) {
        if (cnmt->Read(content) < sizeof(content))
            return;

        programId = content.id;
        type = content.type;
    }
}
