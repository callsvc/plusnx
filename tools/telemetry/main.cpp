#include <filesystem>
#include <vector>
#include <fstream>
#include <array>
#include <print>

#include <cstring>
#include <cassert>

#include <boost/endian/conversion.hpp>

#include <mbedtls/aes.h>
#include <mbedtls/cipher.h>
std::array<std::uint8_t, 16> CalculateCtr(std::uint64_t counter) {
    counter >>= 4;
    std::array<std::uint8_t, 16> result{};
    boost::endian::native_to_big_inplace(counter);
    std::memcpy(&result[8], &counter, sizeof(counter));

    return result;
}

int main(const int argc, char** argv) {
    if (argc < 2) {
        return 1;
    }

    const std::filesystem::path input(argv[1]);
    std::fstream submission(input, std::ios::in);
    if (!submission.is_open()) {
        return 1;
    }

    std::vector<char> content(file_size(input));
    submission.read(content.data(), content.size());

    auto pointer{content.data()};

    while (*pointer != '\0')
        pointer++;
    pointer++;

    struct Content {
        std::array<uint8_t, 16> aes;
        std::uint64_t encrypted;
        std::uint8_t data[];
    };

    auto file{reinterpret_cast<Content*>(pointer)};
    assert(file->encrypted);

    std::vector<char> message(file->encrypted);
    mbedtls_aes_context ctx;
    std::array<std::uint8_t, 16> streaming;

    mbedtls_aes_init(&ctx);
    mbedtls_aes_setkey_enc(&ctx, file->aes.data(), file->aes.size() * 8);

    auto ctr{CalculateCtr(pointer - content.data() + offsetof(Content, data))};
    std::uint64_t block{};
    mbedtls_aes_crypt_ctr(&ctx, file->encrypted, &block, ctr.data(), streaming.data(), file->data, reinterpret_cast<std::uint8_t*>(message.data()));

    mbedtls_aes_free(&ctx);
    const auto final{pointer + file->encrypted};
    assert(std::string_view(final).contains("End of File"));

    std::print("{}\n", std::string_view(message.data(), message.size()));

    return {};
}
