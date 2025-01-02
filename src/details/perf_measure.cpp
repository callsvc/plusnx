#include <print>

#include <details/perf_measure.h>
namespace Plusnx::Details {
    std::optional<std::string> GetMetricFormat(const Metrics format) {
        if (format == Metrics::ClockStyle)
            return std::nullopt;

        std::string result{"s"};
        if (format == Metrics::Milli)
            result.insert(0, "m");

        return result;
    }

    PerfMeasure::PerfMeasure(const Metrics format, const std::string& titleName) : title(titleName), type(format) {
        first = std::chrono::high_resolution_clock::now();
    }
    PerfMeasure::~PerfMeasure() {
#if 0
        std::println("Task {} completed in {}", title, GetElapsed());
#endif
    }

    std::string PerfMeasure::GetElapsed() const {
        const auto second{std::chrono::high_resolution_clock::now()};
        double elapsed{};

        if (type == Metrics::Milli) {
            const auto starts{std::chrono::time_point_cast<std::chrono::microseconds>(first).time_since_epoch().count()};
            const auto finish{std::chrono::time_point_cast<std::chrono::microseconds>(second).time_since_epoch().count()};

            elapsed = (finish - starts) * 1e-3;
        } else {
            const auto starts{std::chrono::time_point_cast<std::chrono::seconds>(first).time_since_epoch().count()};
            const auto finish{std::chrono::time_point_cast<std::chrono::seconds>(second).time_since_epoch().count()};

            elapsed = finish - starts;
        }
        if (auto format{GetMetricFormat(type)})
            return std::format("{}{}", elapsed, *format);

        return {};
    }
}
