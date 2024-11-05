#include <security/cipher_cast.h>

namespace Plusnx::Security {
    std::string GetMbedError(const i32 error) {
        switch (error) {
            case MBEDTLS_ERR_CIPHER_BAD_INPUT_DATA:
                return "Invalid or malformed parameter";
            default:
                return {};
        }
    }
    CipherCast::CipherCast(const u8* key, const u64 size, const OperationMode mode, const bool decrypt) : type(mode) {
        mbedtls_cipher_init(&cortex);

        const auto value{static_cast<mbedtls_cipher_type_t>(mode)};
        const auto info{mbedtls_cipher_info_from_type(value)};

        std::memset(vector.data(), 0, sizeof(vector));
        mbedtls_cipher_setup(&cortex, info);

        if (const auto result = mbedtls_cipher_setkey(&cortex, key, size * 8, decrypt ? MBEDTLS_DECRYPT : MBEDTLS_ENCRYPT))
            throw Except("{}", GetMbedError(result));
        assert(mbedtls_cipher_info_get_key_bitlen(info) == size * 8);
    }
    CipherCast::~CipherCast() {
        mbedtls_cipher_reset(&cortex);
        mbedtls_cipher_free(&cortex);
    }

    void CipherCast::Process(void* dest, const void* src, u64 size) {
        std::optional<std::vector<u8>> container;

        const auto source{static_cast<const u8*>(src)};
        u8* output = [&] {
            if (static_cast<u8*>(dest) != source)
                return static_cast<u8*>(dest);

            if (buffer.empty())
                Resize(size);
            if (buffer.size() >= size)
                return buffer.data();

            container.emplace(size);
            return container->data();
        }();

        for (u64 read{}; read < size; ) {
            const u64 stride{type != OperationMode::EcbAes ? size : mbedtls_cipher_get_block_size(&cortex)};

            mbedtls_cipher_reset(&cortex);
            u64 result{};
            mbedtls_cipher_update(&cortex, &source[read], stride, &output[read], &result);
            read += result;
        }
        if (output != dest)
            std::memcpy(dest, output, size);
    }

    void CipherCast::SetIvValue(const InitVector& iv) {
        vector = iv;
        assert(mbedtls_cipher_set_iv(&cortex, vector.data(), vector.size()) == 0);
    }

    void CipherCast::Decrypt(void* output, const u64 size, u64& sector, const u64 stride) {
        assert(type == OperationMode::XtsAes && mbedtls_cipher_get_operation(&cortex) == MBEDTLS_DECRYPT);

        const auto source{static_cast<u8*>(output)};
        for (u64 pos{}; pos < size; pos += stride) {
            SetIvValue(GetSwitchCounter(sector++));
            Process(&source[pos], &source[pos], stride);
        }
    }

    void CipherCast::Resize(u64 size) {
        // We will accept much smaller or larger sizes when the buffer has not yet been initialized
        constexpr auto MinimumSize{1024 * 1024 * 4};
        constexpr auto MaximumSize{MinimumSize * 32};
        if (!buffer.empty()) {
            if (size < MinimumSize)
                size = MinimumSize;
            if (size > MaximumSize)
                size = MaximumSize;
        }
        buffer.resize(size);
    }
}
