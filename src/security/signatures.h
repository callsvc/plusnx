#pragma once
#include <mbedtls/rsa.h>
#include <types.h>

#define ENB_UNIT_TESTS 1
namespace Plusnx::Security {
    enum class SignatureOperationType {
        None,
        NcaHdrSignatureFixed,
    };

    class Signatures {
    public:
        Signatures(SignatureOperationType operation);
        ~Signatures();

        template <typename T, u64 Size> requires (std::is_trivial_v<T>)
        bool Verify(const T& object, const std::array<u8, Size>& signature) {
            return Verify(reinterpret_cast<const u8*>(&object), sizeof(T), signature.data(), signature.size());
        }
        bool Verify(const u8* input, u64 size, const u8* signature, u64 length);

#if ENB_UNIT_TESTS
        void UpdateRsaContext(const std::span<u8>& modulus, const std::span<u8>& exponent);
        bool VerifyAgainst(const std::span<u8>& hash, const std::span<u8>& content);
#endif
        SignatureOperationType type;
    private:
        std::optional<std::span<const u8>> mask;
        mbedtls_rsa_context context;
    };
}