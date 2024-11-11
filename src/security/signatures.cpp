#include <ranges>
#include <security/checksum.h>
#include <security/key_types.h>

#include <security/signatures.h>
namespace Plusnx::Security {
    constexpr std::array<u8, 256> NcaHdrSign0{0xBF, 0xBF, 0x42, 0x6F, 0xF0, 0xA2, 0x86, 0xEE, 0xF8, 0x74, 0x06, 0x92, 0x6D, 0x10, 0x79, 0x20, 0x86, 0xAD, 0x59, 0x8D, 0x4C, 0x2D, 0x0D, 0x14, 0xB3, 0xA8, 0x6F, 0x52, 0x83, 0x36, 0x53, 0x47, 0x14, 0x91, 0x27, 0x80, 0x51, 0x07, 0x98, 0x3D, 0x17, 0x2A, 0x59, 0x87, 0x5C, 0x45, 0xFF, 0x39, 0x89, 0x35, 0x74, 0x6D, 0x83, 0x32, 0xA7, 0x18, 0x3F, 0xB2, 0x1C, 0xE5, 0xCA, 0x3D, 0x39, 0x8D, 0xF4, 0x10, 0xBA, 0x4E, 0x4E, 0x1B, 0x1E, 0xEA, 0xA3, 0xF5, 0xD0, 0x9D, 0x05, 0xF4, 0x2A, 0xA0, 0xF7, 0xD3, 0xE7, 0x9C, 0x39, 0x25, 0x45, 0xE7, 0x57, 0xDC, 0xAC, 0xF2, 0x54, 0xF7, 0x13, 0x38, 0x06, 0xE6, 0x98, 0xEA, 0x9B, 0x10, 0xF6, 0x7F, 0x76, 0x02, 0x57, 0x82, 0xE6, 0x05, 0xA7, 0x49, 0x74, 0xA8, 0xF2, 0xBD, 0x4B, 0x2B, 0xE4, 0xB9, 0x79, 0x86, 0x7C, 0x40, 0x8E, 0xBC, 0xD7, 0x73, 0x4E, 0x83, 0xED, 0x95, 0x38, 0x17, 0xC4, 0x8D, 0xC9, 0xED, 0x47, 0xD9, 0x3A, 0xB9, 0xC3, 0x21, 0x50, 0xBF, 0x4C, 0x37, 0x4B, 0xB2, 0xFE, 0xAB, 0x59, 0x39, 0xFA, 0x36, 0xDF, 0x6E, 0x62, 0x19, 0x61, 0x9F, 0xCE, 0xE5, 0x53, 0xD9, 0x8F, 0x38, 0x52, 0x54, 0x5A, 0x48, 0x62, 0xC9, 0x49, 0x9A, 0x42, 0x47, 0xE4, 0xE5, 0xDB, 0xD8, 0x47, 0x55, 0xFA, 0x09, 0x39, 0xFB, 0x19, 0x7E, 0x9E, 0x94, 0x0C, 0x5B, 0x6C, 0x09, 0x29, 0x88, 0xB6, 0xF7, 0x60, 0xB9, 0xD6, 0xBB, 0x31, 0xDE, 0xAD, 0xE6, 0xD2, 0xA8, 0x38, 0x01, 0x73, 0x7A, 0xE3, 0xFF, 0xEA, 0xC5, 0xA1, 0x3D, 0xF3, 0xC7, 0x74, 0x9F, 0x9E, 0xD3, 0x6E, 0xC4, 0xA6, 0x1B, 0x64, 0x9F, 0x04, 0xE4, 0x01, 0x0D, 0x6F, 0xA6, 0x83, 0x6E, 0xF4, 0xB1, 0xE8, 0x6D, 0x6E, 0x92, 0x00, 0x85, 0xD1, 0x03, 0xFE, 0x0B, 0x61, 0x19, 0x0A, 0xFC};

    std::map<SignatureOperationType, std::array<u8, 256>> signatures;

    std::array<u8, 256> ProcessSignature(const std::array<u8, 256>& encoded) {
        std::array<u8, 256> result;
        for (auto [index, byte] : std::ranges::views::enumerate(encoded)) {
            result[index] = byte ^ index;
        }
        return result;
    }

#if ENB_UNIT_TESTS
    void Signatures::UpdateRsaContext(const std::span<u8>& modulus, const std::span<u8>& exponent) {
        assert(type == SignatureOperationType::None);
        assert(mbedtls_rsa_import_raw(&context, modulus.data(), modulus.size(), nullptr, 0, nullptr, 0, nullptr, 0, exponent.data(), exponent.size()) == 0);
        assert(mbedtls_rsa_complete(&context) == 0);
    }

    bool Signatures::VerifyAgainst(const std::span<u8>& hash, const std::span<u8>& content) {
        assert(mbedtls_rsa_get_len(&context) == content.size());
        return mbedtls_rsa_rsassa_pss_verify(&context, MBEDTLS_MD_SHA256, hash.size(), hash.data(), content.data()) == 0;
    }
#endif

    Signatures::Signatures(const SignatureOperationType operation) : type(operation) {
        assert(mbedtls_rsa_self_test(0) == 0);
        mbedtls_rsa_init(&context);
        assert(mbedtls_rsa_set_padding(&context, MBEDTLS_RSA_PKCS_V21, MBEDTLS_MD_SHA256) == 0);

        if (signatures.empty()) {
            signatures.emplace(SignatureOperationType::NcaHdrSignatureFixed, ProcessSignature(NcaHdrSign0));
        }
        if (type == SignatureOperationType::NcaHdrSignatureFixed)
            mask.emplace(signatures[type]);

        if (!mask.has_value())
            return;
        constexpr std::array<u8, 3> exponent{1, 0, 1};

        mbedtls_mpi modulus;
        mbedtls_mpi e;
        mbedtls_mpi_init(&modulus);
        mbedtls_mpi_init(&e);

        assert(mbedtls_mpi_read_binary(&modulus, mask->data(), mask->size()) == 0);
        assert(mbedtls_mpi_read_binary(&e, exponent.data(), exponent.size()) == 0);

        assert(mbedtls_rsa_import(&context, &modulus, nullptr, nullptr, nullptr, &e) == 0);
        assert(mbedtls_rsa_complete(&context) == 0);

        mbedtls_mpi_free(&modulus);
        mbedtls_mpi_free(&e);
    }
    Signatures::~Signatures() {
        mbedtls_rsa_free(&context);
    }

    bool Signatures::Verify(const u8 *input, const u64 size, const u8 *signature, const u64 length) {
        assert(mbedtls_rsa_get_len(&context) == length);
        std::vector<u8> hash(0x20);
        if (type != SignatureOperationType::NcaHdrSignatureFixed && !mask.has_value())
            return {};

        Checksum integrity;
        integrity.Update(input, size);
        integrity.Finish(hash);

        return mbedtls_rsa_rsassa_pss_verify(&context, MBEDTLS_MD_SHA256, hash.size(), hash.data(), signature) == 0;
    }

}
