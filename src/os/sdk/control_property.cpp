#include <os/sdk/control_property.h>
namespace Plusnx::Os::Sdk {
    ControlProperty::ControlProperty(const SysFs::FileBackingPtr& nacp) {
        if (nacp->Read(content) != sizeof(ControlHeader))
            throw std::runtime_error("Corrupted NACP file, invalid size");

        languages.reserve(sizeof(u32));
        for (u32 lang{}; lang < Services::Settings::LastIncludedLanguageId; lang++) {
            if (content.supportedLanguages & 1 << lang)
                languages.emplace_back(lang);
        }
        titleId = content.presenceGroup;
    }

    std::string ControlProperty::GetApplicationName(Services::Settings::LanguageType type) const {
        const auto application{Services::Settings::GetLanguageCode(static_cast<u32>(type), true)};
        if (!ContainsValue(languages, application)) {
            return std::string(content.title[0].titleTag.data());
        }

        const auto& title{content.title.at(application).titleTag};
        return std::string(title.data());
    }

    std::string ControlProperty::GetApplicationPublisher(Services::Settings::LanguageType type) const {
        const auto application{Services::Settings::GetLanguageCode(static_cast<u32>(type), true)};
        if (!ContainsValue(languages, application)) {
            return std::string(content.title[0].publisher.data());
        }

        const auto& publisher{content.title.at(application).publisher};
        return std::string(publisher.data());
    }

    std::string ControlProperty::GetDisplayVersion() const {
        return std::string(content.displayVersion.data());
    }

}
