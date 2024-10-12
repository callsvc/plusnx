#include <print>
#include <unistd.h>

#include <application.h>
#include <context.h>

namespace Plusnx {
    Application::Application() : context(std::make_shared<Context>()) {

        std::print("New application started on core {} with PID {}", sched_getcpu(), getpid());
        assets = std::make_shared<SysFs::Assets>(context);
    }
}
