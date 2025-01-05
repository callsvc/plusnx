#pragma once

#include <chrono>
#include <types.h>
namespace Plusnx::SysFs::FSys {
    class RigidDirectory;
}
namespace Plusnx::Details {
    enum class Metrics {
        Seconds,
        Milli,
        ClockStyle
    };
    using TimePoint = std::chrono::high_resolution_clock::time_point;

    class PerfMeasure {
    public:
        PerfMeasure(Metrics format, u64 program);
        std::optional<std::string> GetElapsed();

        void StartFrame();
        void EndFrame();
        void WriteToFile(SysFs::FSys::RigidDirectory& dir);

    private:
        double GetDeltaMillis(TimePoint _tmp = {});
        double GetDeltaSeconds(TimePoint _tmp = {});

        double (PerfMeasure::* callback)(TimePoint _tmp);

        u64 title;
        Metrics type;
        TimePoint first, end;
        std::vector<std::pair<TimePoint, const std::string>> records;
    };
}
