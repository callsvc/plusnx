#pragma once
#include <sstream>

#include <sys_fs/fs_types.h>
namespace Plusnx::Core {
    class TelemetryCollector {
    public:
        void Query();
        void CommitToFile(const SysFs::FileBackingPtr& output) const;

    private:
        std::stringstream strings;
    };
}
