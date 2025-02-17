#include <chrono>
#include <iostream>
#include <filesystem>
#include <fstream>
#include <print>

#include <boost/program_options.hpp>

template <typename... Args>
void quit(const std::format_string<Args...> format, Args&&... args) {
    std::print(format, std::forward<Args>(args)...);
    std::print("\n");
    std::terminate();
}

std::uint32_t seek{},
    count{},
    col{12};

std::int32_t main(const std::int32_t argc, char** argv) {
    namespace po = boost::program_options;

    po::options_description options("Bin2Array options");
    std::string symbol;
    options.add_options()
        ("usage", "Display the application options")
        ("seek", po::value<std::uint32_t>(&seek), "Advance N bytes in the specified binary")
        ("count", po::value<std::uint32_t>(&count), "Number of bytes to read")
        ("col", po::value<std::uint32_t>(&col), "Column width")
        ("symbol-name", po::value<std::string>(&symbol), "Symbol name")
        ("input", po::value<std::string>(), "Specify the target binary file")
        ("output", po::value<std::string>(), "Specify the output file, it must end with .h or .hpp");

    po::variables_map vm;
    store(parse_command_line(argc, argv, options), vm);
    vm.notify();
    if (vm.contains("usage") || (!vm.contains("input") || !vm.contains("output"))) {
        std::cout << options << std::endl;
        return EXIT_SUCCESS;
    }

    if (symbol.empty())
        symbol = "BinaryData";

    const std::filesystem::path binary{vm["input"].as<std::string>()};
    const std::filesystem::path output{vm["output"].as<std::string>()};

    if (!exists(binary)) {
        quit("The specified input file does not exist");
    }
    std::fstream in(binary, std::ios::binary | std::ios::in);
    std::fstream out(output, std::ios::out | std::ios::trunc);

    if (in.fail() || out.fail()) {
        quit("Failed to open the input file");
    }
    auto inSize{file_size(binary)};
    if (inSize < seek + count) {
        quit("The binary file {} is smaller than the target amount of bytes to be read", binary.string());
    }
    inSize -= seek;
    if (count)
        inSize = count;
    in.seekg(seek);

    auto generate{std::chrono::system_clock::now()};
    out << std::format("// {}\n", generate);

    out << "#include <array>\n#include <cstdint>\n\n";
    out << "using u8 = std::uint8_t;\n\n";

    const auto constExpression{std::format("{}Size", symbol)};
    out << std::format("constexpr auto {}", constExpression);
    out << "{";
    out << std::format("{:#x}", inSize);
    out << "};\n\n";

    out << std::format("std::array<u8, {}> {} ", constExpression, symbol);
    out << "{";

    std::streamsize br{}, offset{};
    std::streamsize bufferSz{count ? count : 4096};
    std::vector<char> buffer(bufferSz);
    do {
        in.read(&buffer[0], bufferSz);
        br = in.gcount();
        if (!br)
            continue;

        for (decltype(br) index{}; index < br; index++) {
            if (!offset)
                out << "\n\t";
            else if (!(offset % col))
                out << "\n\t";
            out << std::format("{:#04X}, ", buffer[index]).replace(1, 1, "x");
            offset++;
        }
        if (count)
            if (offset >= count)
                br = {};

    } while (br > 0);

    out << "\n};\n";
    in.close();

    return {};
}