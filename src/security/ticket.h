#pragma once

#include <sys_fs/fs_types.h>
#include <security/key_types.h>
namespace Plusnx::Security {
    enum class TicketKeyType : u8 {
        Common,
        Personalized
    };

#pragma pack(push, 1)
    struct TicketData {
        std::array<char, 0x40> issuer;
        std::array<u8, 0x100> titleKeyBlock;
        u8 version; // Always 2 for Switch (ES) Tickets
        TicketKeyType keyType;
        u16 ticketVersion;
        u8 licenseType;
        u8 keyRevision;
        u16 properties;
        u64 pad0;
        u64 tickedId;
        u64 deviceId;
        std::array<u8, 0x10> rightsId;
        u32 accountId;
    };
#pragma pack(pop)

    // https://switchbrew.org/wiki/Ticket
    class Ticket {
    public:
        Ticket(const SysFs::FileBackingPtr& ticket);
        K128 GetTitleKey() const;
        std::pair<u64, K128> GetRights() const ;

    private:
        TicketData content;
    };
}