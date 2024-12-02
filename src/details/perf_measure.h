#pragma once

#include <chrono>
namespace Plusnx::Details {
    enum class Metrics {
        Seconds,
        Milli,
        ClockStyle
    };
    class PerfMeasure {
        public:
        PerfMeasure(Metrics format, const std::string& titleName = "Ticks");
        void Stop(double& result, bool output = false);

    private:
        std::string title;
        Metrics type;
        std::chrono::high_resolution_clock::time_point first;
    };
}