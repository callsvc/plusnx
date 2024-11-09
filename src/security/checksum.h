#pragma once

#include <mbedtls/sha256.h>
#include <types.h>
namespace Plusnx::Security {
    enum class CheckingStatus {
        None,
        Updating,
        Finished
    };
    class Checksum {
    public:
        Checksum(bool test = false);
        ~Checksum();
        Checksum(const Checksum& clone);

        void Update(const void* input, u64 size);
        void Finish(const std::span<u8>& result);
    private:
        CheckingStatus status{};
        mbedtls_sha256_context state{};
    };
}