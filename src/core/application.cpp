#include <print>
#include <unistd.h>

#include <core/application.h>
#include <core/context.h>

namespace Plusnx::Core {
    Application::Application() :
        context(std::make_shared<Context>()) {

        std::print("New application started on core {} with PID {}\n", sched_getcpu(), getpid());
        assets = std::make_shared<SysFs::Assets>(context);
    }
    void Application::Initialize(const Video::Vk::VkSupport& support) {
        context->gpu->InitGraphics(support);

        kernel = std::make_shared<Kmo::Kernel>();
        entity = std::make_shared<Kmo::Types::KProcess>(*kernel);
        if (const auto process = context->process.lock())
            process->Destroy();

        context->process = entity;

        if (const auto process = context->process.lock())
            process->Initialize();

        games = std::make_unique<GamesLists>(assets->games);
        games->Initialize();
    }
}
