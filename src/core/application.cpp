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

    Application::Application() : context(std::make_shared<Context>()) {
        std::print("New application started on core {} with PID {}\n", sched_getcpu(), getpid());

        context->keys = std::make_shared<Security::Keyring>(context);

        games = std::make_unique<GamesLists>(context->assets->games);
        games->Initialize();

        appQol = std::make_unique<ProcessQol>(context->assets->GetPlusnxFilename(SysFs::PlusnxDirectoryType::Database));
    }

    Application::~Application() {
        if (ui) {
            auto gui{std::move(ui)};
        }
    }

    void Application::Initialize(std::shared_ptr<Video::GraphicsSupportContext>&& support) {
        context->kernel = std::make_shared<GenericKernel::Kernel>();
        context->gpu->Initialize(support);

        ui = std::move(support);

        context->process = [&] {
            if (const auto last = context->process)
                last->Destroy();

            auto process{context->kernel->CreateNewProcess()};
            process->Initialize();
            return process;
        }();

        context->nxOs = std::make_shared<Os::NxSys>(context);
    }

    void Application::LoadAGameByIndex(const u64 index) const {
        if (!index)
            context->nxOs->LoadApplicationFile(*games->first);
        else
            if (const auto collection = games->GetAllGames(); !collection.empty())
                if (collection.size() < index)
                    context->nxOs->LoadApplicationFile(collection[index]);

        if (context->details)
            appQol->ChangeGame(*context->details);

        for (const auto& recent : appQol->GetPlayedSessions(10)) {
            std::print("Section registered as: {}\n", recent);
        }
    }

    bool Application::PickByName(const std::string& game) {
        const auto& pack{games->GetAllGames()};
        std::optional<boost::regex> regex;

        try {
            regex.emplace(game, boost::regex::grep);
            declared = game;
        } catch (const boost::regex_error& except) {
            std::print("{} is not a valid regex expression, error: {}\n", game, except.what());
            return {};
        }

        std::vector<SysFs::SysPath> filter;
        for (const auto& disc : pack) {
            if (regex_search(disc.string(), *regex))
                filter.emplace_back(disc);
        }
        if (!filter.empty())
            chosen = filter.front();

        return !chosen.empty();
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

        const SysFs::SysPath gamePath{std::format("{:016X}", loader->titleId)};
        return loader->ExtractFilesInto(!gamePath.empty() ? gamePath : declared.filename());
    }

    void Application::SaveUserInformation() const {
        TelemetryCollector collector;
        collector.Query();

        const auto target{context->assets->GetPlusnxFilename(SysFs::PlusnxDirectoryType::Telemetry)};
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
