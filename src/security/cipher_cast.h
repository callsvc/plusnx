#pragma once
#include <mbedtls/cipher.h>

#include <security/key_types.h>
namespace Plusnx::Security {
    enum class OperationMode {
        XtsAes = MBEDTLS_CIPHER_AES_128_XTS, // Counter with IV
        CtrAes = MBEDTLS_CIPHER_AES_128_CTR,
        EcbAes = MBEDTLS_CIPHER_AES_128_ECB, // Insecure mode, without IV
    };
    class CipherCast {
    public:
        CipherCast(const u8* key, u64 size, OperationMode mode, bool decrypt);
        ~CipherCast();

        void Process(void* dest, const void* src, u64 size);

        // Stride refers to the sector size in bytes
        void Decrypt(void* output, u64 size, u64& sector, u64 stride);
        void Resize(u64 size);

        void SetIvValue(const InitVector& iv);

        template <typename T> requires std::is_trivial_v<T>
        void Decrypt(T& object, u64& sector, const u64 stride) {
            Decrypt(reinterpret_cast<void*>(&object), sizeof(object), sector, stride);
        }
    private:
        mbedtls_cipher_context_t cortex;
        std::vector<u8> buffer;

        OperationMode type;
        InitVector vector; // Our initialization vector
    };
}
