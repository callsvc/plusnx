#include <application.h>

namespace Plusnx {
    Application::Application(const std::shared_ptr<Context>& ctx) : context(ctx), assets(std::make_shared<SysFs::Assets>(context)) {
    }
}
