#pragma once
#include <types.h>

namespace Plusnx::Services::Settings {
    constexpr auto LastIncludedLanguageId{17};
    // https://switchbrew.org/wiki/Settings_services#Language

    enum class LanguageType : u32 {
        Japanese,
        AmericanEnglish,
        French,
        German,
        Italian,
        Spanish,
        Chinese,
        Korean,
        Dutch,
        Portuguese,
        Russian,
        Taiwanese,
        BritishEnglish,
        CanadianFrench,
        LatinAmericanSpanish,
        SimplifiedChinese,
        TraditionalChinese,
        BrazilianPortuguese
    };

    struct Language {
        LanguageType type;
        u64 code;
        u32 applicationIndex; // Type index, however commonly specified by applications—yes, they differ
    };

    u32 GetLanguageCode(u32, bool application);
}
