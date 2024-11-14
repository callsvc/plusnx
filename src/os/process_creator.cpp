#include <os/nx_sys.h>
#include <os/process_creator.h>
#include <print>

namespace Plusnx::Os {
    void ProcessCreator::DumpControlContent() const {
        const auto tempDir{std::filesystem::temp_directory_path()};
        if (romfs && nacp)
            romfs->ExtractAllFiles(tempDir / std::format("{:X}", nacp->titleId));
    }

    void ProcessCreator::Initialize() {
        const auto application{nxOs.application};

        if (const auto content{application->GetRomFs(true)})
            romfs = content;

        if (romfs) {
            nacp.emplace(romfs->OpenFile("/control.nacp"));
            const auto& context{nxOs.context};
            title = nacp->GetApplicationName(context->language);
            publisher = nacp->GetApplicationPublisher(context->language);
            version = nacp->GetDisplayVersion();
        }
        if (nacp && nacp->titleId)
            programId = nacp->titleId;

#if 1
        if (nxOs.context->configs->excfs)
            DumpControlContent();
#endif

        std::print("Name of the application about to be loaded: {} v{}\n", title, version);
        std::print("Name of the respective publisher: {}\n", publisher);
        application->Load(nxOs.context);
    }
}
