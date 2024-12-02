#include <array>
#include <vector>
#include <cstdint>
#include <cstdint>
#include <fstream>
#include <ranges>
#include <iostream>

constexpr std::array<std::uint8_t, 256> Data{};

std::vector<std::uint8_t> Process() {
    std::vector<std::uint8_t> content(Data.size());
    for (const auto [index, byte] : std::views::enumerate(Data)) {
        content[index] = byte ^ index;
    }
    return content;
}

int main(const int argc, char** argv) {
    const auto input{Process()};
    if (argc == 2) {
        std::fstream output(argv[1], std::ios::out | std::ios::trunc);
        output.write(reinterpret_cast<const char*>(input.data()), input.size());
    } else {
        std::cout.write(reinterpret_cast<const char*>(input.data()), input.size());
    }
}

