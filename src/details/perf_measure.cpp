#include <print>
#include <ranges>

#include <sys_fs/fsys/rigid_directory.h>
#include <sys_fs/streamed_file.h>
#include <details/perf_measure.h>

#include <fmt/chrono.h>
namespace Plusnx::Details {
    PerfMeasure::PerfMeasure(const Metrics format, const u64 program) : title(program), type(format) {
        first = std::chrono::high_resolution_clock::now();
        end = {};
        callback = type == Metrics::Milli ? &PerfMeasure::GetDeltaMillis : &PerfMeasure::GetDeltaSeconds;
        records.reserve(120);
    }

    std::optional<std::string> PerfMeasure::GetElapsed() {
        if (type == Metrics::ClockStyle)
            return {};

        auto result = [&] {
            const auto delta{(this->*callback)(first)};
            if (type == Metrics::Milli)
                return std::format("{:016X}: {}ms", title, std::to_string(delta));
            return std::format("{:016X}: {:02}s", title, delta);
        }();
        return result;
    }

    void PerfMeasure::StartFrame() {
        first = std::chrono::high_resolution_clock::now();
    }

    void PerfMeasure::EndFrame() {
        if (!records.empty())
            if (records.back().first == first)
                return;
        if (const auto elapsed{GetElapsed()})
            records.emplace_back(first, std::move(*elapsed));
    }

    void PerfMeasure::WriteToFile(SysFs::FSys::RigidDirectory& dir) {
        const SysFs::FileBackingPtr file = [&] {
            const auto statics{std::format("Statistics-{:016X}.csv", title)};
            if (auto fileStatics{dir.OpenFile(statics, SysFs::FileMode::Write)})
                return fileStatics;
            return dir.CreateFile(statics);
        }();

        if (!file)
            return;
        SysFs::StreamedFile writable(file, file->GetSize());

        std::vector<char> buffer;
        buffer.reserve(records.size() * 0x40);
        if (!file->GetSize()) {
            fmt::format_to(std::back_inserter(buffer), "sep=,\n");
        }
        for (const auto& [index, _record] : std::ranges::views::enumerate(records)) {
            fmt::format_to(std::back_inserter(buffer), "{}, {:%D %T}, {},\n", index, _record.first, _record.second.substr(0, 16));
        }
        writable.PutBytes(buffer);
    }

    double PerfMeasure::GetDeltaMillis(const TimePoint _tmp) {
        first = _tmp;
        end = std::chrono::high_resolution_clock::now();
        const auto starts{std::chrono::time_point_cast<std::chrono::microseconds>(first).time_since_epoch().count()};
        const auto finish{std::chrono::time_point_cast<std::chrono::microseconds>(end).time_since_epoch().count()};
        return finish - starts * 1e-3;
    }

    double PerfMeasure::GetDeltaSeconds(const TimePoint _tmp) {
        first = _tmp;
        end = std::chrono::high_resolution_clock::now();

        const auto starts{std::chrono::time_point_cast<std::chrono::seconds>(first).time_since_epoch().count()};
        const auto finish{std::chrono::time_point_cast<std::chrono::seconds>(end).time_since_epoch().count()};

        return finish - starts;
    }
}
