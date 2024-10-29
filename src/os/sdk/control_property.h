#pragma once
#include <string>
#include <services/settings/languages.h>

#include <sys_fs/fs_types.h>
namespace Plusnx::Os::Sdk {
#pragma pack(push, 1)
    struct ApplicationTitle {
        std::array<char, 0x200> titleTag;
        std::array<char, 0x100> publisher;
    };
    struct alignas(0x4000) ControlHeader {
        std::array<ApplicationTitle, 0x10> title;
        std::array<u8, 0x2c> pad0;
        u32 supportedLanguages;
        std::array<u8, 0x8> pad1;
        u64 presenceGroup;
        std::array<u8, 0x20> pad2;
        std::array<char, 0x10> displayVersion;
        u64 addonContentBaseId;
        u64 saveDataOwnerId; // Describes the ID of the directory containing the application's dynamic data

        static_assert(sizeof(title) == 0x300 * 0x10);
    };
#pragma pack(pop)

    // https://switchbrew.org/wiki/NACP#ApplicationTitle
    class ControlProperty {
    public:
        ControlProperty(const SysFs::FileBackingPtr& nacp);

        std::string GetApplicationName(Services::Settings::LanguageType type) const;
        std::string GetApplicationPublisher(Services::Settings::LanguageType type) const;
        std::string GetDisplayVersion() const;

        u64 titleId;
        std::vector<u32> languages;
    private:
        ControlHeader content;
    };
}
