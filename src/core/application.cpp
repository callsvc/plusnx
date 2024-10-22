#include <print>
#include <unistd.h>

#include <core/application.h>
#include <core/context.h>

#include <os/nx_sys.h>
namespace Plusnx::Core {
    Application::Application() :
        context(std::make_shared<Context>()) {
        std::print("New application started on core {} with PID {}\n", sched_getcpu(), getpid());

        assets = std::make_shared<SysFs::Assets>(context);
        kernel = std::make_shared<Gsk::Kernel>();
        kproc = std::make_shared<Gsk::Types::KProcess>(*kernel);

        games = std::make_unique<GamesLists>(assets->games);
        games->Initialize();
    }

    void Application::Initialize(const Video::Vk::VkSupport& support) {
        context->gpu->InitGraphics(support);

        context->process = [&] {
            if (const auto process = context->process.lock())
                process->Destroy();

            kproc->Initialize();
            return kproc;
        }();
        nos = std::make_shared<Os::NxSys>(context);
    }

    void Application::LoadAGameByIndex(const u64 index) const {
        if (!index)
            nos->LoadApplicationFile(*games->first);
        else
            if (const auto collection = games->GetAllGames(); !collection.empty())
                if (collection.size() < index)
                    nos->LoadApplicationFile(collection[index]);
    }
}
