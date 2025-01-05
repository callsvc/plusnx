#include <thread>
#include <details/perf_measure.h>
#include <sys_fs/fsys/rigid_directory.h>

Plusnx::i32 main() {
    Plusnx::Details::PerfMeasure measure(Plusnx::Details::Metrics::Seconds, 0);
    for (const auto _ : Plusnx::Range<Plusnx::u32>(0, 10).CreateValues()) {
        measure.StartFrame();
        std::this_thread::sleep_for(std::chrono::seconds(1));

        measure.EndFrame();
    }
    Plusnx::SysFs::FSys::RigidDirectory _thisDirectory{};
    measure.WriteToFile(_thisDirectory);

    return {};
}
