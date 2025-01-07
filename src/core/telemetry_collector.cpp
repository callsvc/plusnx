#include <unistd.h>
#include <sys/sysinfo.h>

#include <mbedtls/pk.h>
#include <mbedtls/ctr_drbg.h>

#include <types.h>
#include <sys_fs/ctr_backing.h>
#include <sys_fs/streamed_file.h>
#include <core/telemetry_collector.h>
namespace Plusnx::Core {
    u64 GetCoresTotal() {
        const auto processors{static_cast<u64>(get_nprocs())};
        const auto byConfig{static_cast<u64>(sysconf(_SC_NPROCESSORS_ONLN))};

        return std::max(processors, byConfig);
    }
    void TelemetryCollector::Query() {
        strings << std::format("Available core count: {}\n", GetCoresTotal());
    }

    const auto seeder{"Plusnx Random AES"};

    void TelemetryCollector::CommitToFile(SysFs::FileBackingPtr&& output) const {
        mbedtls_entropy_context ent;
        mbedtls_ctr_drbg_context ctr;

        mbedtls_entropy_init(&ent);
        mbedtls_ctr_drbg_init(&ctr);

        Security::K128 result{};

        mbedtls_ctr_drbg_seed(&ctr, mbedtls_entropy_func, &ent, reinterpret_cast<const u8*>(seeder), strlen(seeder));
        mbedtls_ctr_drbg_random(&ctr, result.data(), result.size());

        mbedtls_ctr_drbg_free(&ctr);
        mbedtls_entropy_free(&ent);

        const auto& visible{strings.str()};

        SysFs::StreamedFile writable(std::move(output));
        writable << "Data submission file";
        writable.Write(result.data(), result.size());
        writable << visible.size();

        {
            SysFs::CtrBacking cipher(output, result, writable.GetCursor(SysFs::CounterType::Write));
            cipher.Write(visible.data(), visible.size());
        }

        writable.SkipBytes(visible.size(), SysFs::CounterType::Write);
        writable << "End of File";
    }
}
