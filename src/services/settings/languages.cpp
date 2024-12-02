#include <services/settings/languages.h>

namespace Plusnx::Services::Settings {
    std::vector<Language> languages;

    void PopulateLanguages() {
        languages.clear();
        languages.emplace_back(LanguageType::Japanese, ConstMagic<u64>("ja"), 2);
        languages.emplace_back(LanguageType::AmericanEnglish, ConstMagic<u64>("en-US"), 0);
    }

    // Converts system language to application language and vice versa
    u32 GetLanguageCode(const u32 type, const bool application) {
        if (languages.empty())
            PopulateLanguages();
        if (application) {
            return languages.at(type).applicationIndex;
        }
        for (const auto& supported : languages) {
            if (supported.applicationIndex == type)
                return static_cast<u32>(supported.type);
        }
        __builtin_trap();
    }
}

