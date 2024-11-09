#include <print>
#include <unistd.h>
#include <boost/regex.hpp>

#include <security/keyring.h>
#include <core/context.h>

#include <os/nx_sys.h>
#include <os/make_loader.h>

#include <core/telemetry_collector.h>
#include <core/application.h>
namespace Plusnx::Core {
    Application::Application() :
        context(std::make_shared<Context>()) {
        std::print("New application started on core {} with PID {}\n", sched_getcpu(), getpid());

        assets = std::make_shared<SysFs::Assets>(context);
        kernel = std::make_shared<GenericKernel::Kernel>();

        games = std::make_unique<GamesLists>(assets->games);
        games->Initialize();

        context->keys = std::make_shared<Security::Keyring>(context);
    }

    Application::~Application() {
        if (ui) {
            auto gui{std::move(ui)};
        }
    }

    void Application::Initialize(std::shared_ptr<Video::GraphicsSupportContext>&& support) {
        context->gpu->Initialize(support);

        context->process = [&] {
            if (const auto last = context->process)
                last->Destroy();

            auto process{kernel->CreateNewProcess()};
            process->Initialize();
            return process;
        }();

        ui = std::move(support);
        context->nxOs = std::make_shared<Os::NxSys>(context);
    }

    void Application::LoadAGameByIndex(const u64 index) const {
        if (!index)
            context->nxOs->LoadApplicationFile(*games->first);
        else
            if (const auto collection = games->GetAllGames(); !collection.empty())
                if (collection.size() < index)
                    context->nxOs->LoadApplicationFile(collection[index]);
    }

    void Application::PickByName(const std::string& game) {
        const auto& pack{games->GetAllGames()};
        declared = game;

        const boost::regex regex(game);
        std::vector<SysFs::SysPath> filter;
        for (const auto& disc : pack) {
            if (regex_match(disc.string(), regex))
                filter.emplace_back(disc);
        }
        if (!filter.empty())
            chosen = filter.front();
    }

    bool Application::ExtractIntoGameFs() {
        if (chosen.empty())
            return {};

        const auto type{Os::GetAppTypeByFilename(chosen)};
        if (type != Loader::AppType::Nsp && type != Loader::AppType::Xci)
            return {};

        const auto loader{Os::MakeLoader(context, std::make_shared<SysFs::FSys::RegularFile>(chosen))};
        if (!loader)
            return {};

        return loader->ExtractFilesInto(SysFs::SysPath(declared).filename());
    }

    void Application::SaveUserInformation() const {
        TelemetryCollector collector;
        collector.Query();

        const auto target{assets->temp.path / "telemetry.enc"};
        const auto outputFile{context->provider->CreateSystemFile(SysFs::RootId, target)};
        collector.CommitToFile(outputFile);
    }

    void Application::ClearUiEvents() const {
        if (ui->apiType == Video::ApiType::Sdl) {
            SDL_PumpEvents();
        }
    }

    void Application::UpdateFrame() const {
        ui->Update();
    }
}
