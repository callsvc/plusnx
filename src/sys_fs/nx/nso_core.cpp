#include <boost/regex.hpp>

#include <lz4.h>
#include <ranges>

#include <security/checksum.h>
#include <sys_fs/nx/nso_core.h>
namespace Plusnx::SysFs::Nx {
    NsoCore::NsoCore(const FileBackingPtr& nso) : backing(nso) {
        if (nso->Read(content) < sizeof(content))
            return;
        assert(content.magic == ConstMagic<u32>("NSO0"));

        program.resize(content.data.offset + content.data.size);

        ReadSection(content.text, content.textFileSize, textSection);
        ReadSection(content.ro, content.roFileSize, roSection);
        ReadSection(content.data, content.dataFileSize, dataSection);

        // The idea of extracting these strings for debugging came from the Ryujinx project
        const auto readable{std::string(reinterpret_cast<char*>(roSection.data()), roSection.size())};

        std::string modulePath;
        constexpr u32 empty{};
        if (std::memcmp(readable.data(), &empty, sizeof(empty)) == 0) {
            u32 length{};
            std::memcpy(&length, readable.data() + sizeof(length), sizeof(length));
            modulePath.assign(readable.data() + 8, length);
        }

        boost::match_results<const char*> matches;
        if (modulePath.empty()) {
            const boost::regex moduleRegex("[a-z]:[\\/][ -~]{5,}\\.nss", boost::regex::icase);
            if (regex_search(readable.data(), matches, moduleRegex))
                modulePath = matches.str();
        }

        if (!modulePath.empty()) {
            std::print("Module built: {}\n", modulePath);
            moduleName.assign(modulePath);
        } else {
            moduleName = backing->path;
        }

        const boost::regex fsSdkRegex("sdk_version: ([0-9.]*)");
        if (regex_search(readable.data(), matches, fsSdkRegex)) {
            std::print("FS SDK version: {}\n", matches.str());
        }

        const boost::regex sdkMwRegex("SDK MW[ -~]*");

        std::vector<std::string> sdks;
        boost::sregex_iterator sdkMatches(readable.begin(), readable.end(), sdkMwRegex);
        for (; sdkMatches != decltype(sdkMatches)(); ++sdkMatches)
            sdks.emplace_back(sdkMatches->str());

        if (sdks.size()) {
            std::print("SDK Libraries: ");
            for (const auto& sdk : sdks)
                std::print("{} ", sdk);

            std::println();
        }

        if (content.moduleNameSize && moduleName.empty()) {
            const auto module{backing->GetBytes<char>(content.moduleNameSize, content.moduleNameOffset)};
            moduleName.assign(module.data());
        }
    }

    void NsoCore::Load(const u64& vAddr, const bool linker) const {
        i32 count{};
        for (const auto& [type, sectionHash] : sectionResults) {
            if (!IsEqual(sectionHash, content.hashList[std::to_underlying(type)]))
                return;
            if (std::to_underlying(type) != count)
                return;
            count++;
        }
        if (count < 3)
            return;

        if (linker && vAddr) {}
    }

    void NsoCore::ReadSection(const NsoSection& section, const u64 fileSize, std::span<u8>& output) {
        u64 sectionAddr{};
        sectionAddr += textSection.size();
        sectionAddr += roSection.size();

        const auto type{GetSectionType(section)};
        assert(dataSection.size() == 0);
        output = std::span(program.data() + sectionAddr, section.size);

        assert(program.size() >= sectionAddr + output.size());

        backing->Read(output.data(), fileSize, section.fileOffset);

        if (fileSize < section.size || IsSectionCompressed(type)) {
            std::vector<u8> compressed(fileSize);
            std::memcpy(compressed.data(), output.data(), fileSize);

            const auto stream{LZ4_createStreamDecode()};
#if 0
            std::print("LZ4 library version: {}\n", LZ4_versionString());
#endif
            const auto result{LZ4_decompress_safe_continue(stream, reinterpret_cast<char*>(compressed.data()), reinterpret_cast<char*>(output.data()), compressed.size(), output.size())};

            if (result < 0)
                return;
            LZ4_freeStreamDecode(stream);
        }

        std::array<u8, 0x20> result{};
        if (HashableSection(type)) {
            Security::Checksum verifier;
            verifier.Update(output.data(), output.size());
            verifier.Finish(result);
        }
        sectionResults.emplace(type, result);

    }

    Loader::SectionType NsoCore::GetSectionType(const NsoSection& section) const {
        if (!std::memcmp(&content.text, &section, sizeof(section)))
            return Loader::SectionType::Text;
        if (!std::memcmp(&content.ro, &section, sizeof(section)))
            return Loader::SectionType::Ro;
        if (!std::memcmp(&content.data, &section, sizeof(section)))
            return Loader::SectionType::Data;

        return Loader::SectionType::Invalid;
    }

    bool NsoCore::IsSectionCompressed(Loader::SectionType type) const {
        assert(static_cast<u32>(type) < 3);
        return content.flags >> std::to_underlying<Loader::SectionType>(type) & 1;
    }
    bool NsoCore::HashableSection(Loader::SectionType type) const {
        assert(static_cast<u32>(type) < 3);
        // ReSharper disable once CppRedundantParentheses
        return content.flags >> (std::to_underlying<Loader::SectionType>(type) + 3) & 1;
    }
}
