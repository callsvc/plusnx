#include <print>

#include <details/perf_measure.h>
namespace Plusnx::Details {
    PerfMeasure::PerfMeasure(const Metrics format, const std::string& titleName) : title(titleName), type(format) {
        first = std::chrono::high_resolution_clock::now();
    }
    std::optional<std::string> GetMetricFormat(const Metrics format) {
        if (format == Metrics::Milli)
            return "ms";
        if (format == Metrics::Seconds)
            return "s";

        return {};
    }

    void PerfMeasure::Stop(double& result, const bool output) {
        const auto second{std::chrono::high_resolution_clock::now()};

        if (type == Metrics::Milli) {
            const auto starts{std::chrono::time_point_cast<std::chrono::microseconds>(first).time_since_epoch().count()};
            const auto finish{std::chrono::time_point_cast<std::chrono::microseconds>(second).time_since_epoch().count()};

            result = (finish - starts) * 1e-3;
        } else {
            const auto starts{std::chrono::time_point_cast<std::chrono::seconds>(first).time_since_epoch().count()};
            const auto finish{std::chrono::time_point_cast<std::chrono::seconds>(second).time_since_epoch().count()};

            result = finish - starts;
        }
        if (!output)
            return;

        if (auto format{GetMetricFormat(type)})
            std::print("Task {} completed in {}{}\n", title, result, *format);
    }
}
