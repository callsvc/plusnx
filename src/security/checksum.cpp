#include <mbedtls/md.h>

#include <security/checksum.h>
namespace Plusnx::Security {
    Checksum::Checksum(const bool test) {
        mbedtls_sha256_init(&state);
        if (test) {
            if (mbedtls_sha256_self_test(1))
                return;
        }

        status = CheckingStatus::Finished;
    }
    void Checksum::Update(const void* input, const u64 size) {
        if (status == CheckingStatus::Finished)
            mbedtls_sha256_starts(&state, 0);

        assert(mbedtls_sha256_update(&state, static_cast<const u8*>(input), size) == 0);
        status = CheckingStatus::Updating;
    }

    void Checksum::Finish(const std::span<u8>& result) {
        if (status != CheckingStatus::Updating)
            return;
        assert(result.size() == mbedtls_md_get_size(mbedtls_md_info_from_type(MBEDTLS_MD_SHA256)));
        mbedtls_sha256_finish(&state, result.data());
        status = CheckingStatus::Finished;
    }

    Checksum::~Checksum() {
        mbedtls_sha256_free(&state);
    }

    Checksum::Checksum(const Checksum& clone) {
        mbedtls_sha256_clone(&state, &clone.state);
        status = clone.status;
    }
}
