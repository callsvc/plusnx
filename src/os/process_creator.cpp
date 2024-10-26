#include <os/nx_sys.h>
#include <os/process_creator.h>

namespace Plusnx::Os {
    void ProcessCreator::DumpRomContent() {
        const auto tempDir{std::filesystem::temp_directory_path()};
        if (romFs)
            romFs->ExtractAllFiles(tempDir);
    }

    void ProcessCreator::Initialize() {
        const auto application{nxOs.application};

        if (auto stream{application->GetMainRomFs()})
            romFs.emplace(stream);
#if 1
        DumpRomContent();
#endif
        application->Load(nxOs.context);
    }
}
