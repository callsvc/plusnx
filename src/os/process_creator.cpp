#include <print>
#include <os/nx_sys.h>
#include <os/process_creator.h>

namespace Plusnx::Os {
    void ProcessCreator::DumpRomContent() const {
        const auto tempDir{std::filesystem::temp_directory_path()};
        if (romfs)
            romfs->ExtractAllFiles(tempDir);
    }

    void ProcessCreator::Initialize() {
        const auto application{nxOs.application};

        if (auto content{application->romfs})
            romfs = content;

        if (auto content{application->control}) {
            nacp.emplace(content);
            const auto& context{nxOs.context};
            title = nacp->GetApplicationName(context->language);
            publisher = nacp->GetApplicationPublisher(context->language);
            version = nacp->GetDisplayVersion();
        }
        if (nacp && nacp->titleId)
            programId = nacp->titleId;

#if 1
        DumpRomContent();
#endif

        std::print("Name of the application about to be loaded: {} v{}\n", title, version);
        std::print("Name of the respective publisher: {}\n", publisher);
        application->Load(nxOs.context);
    }
}
