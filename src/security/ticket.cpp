#include <boost/align/align_up.hpp>
#include <security/ticket.h>
namespace Plusnx::Security {
    enum class SignatureType : u32 {
        Rsa4096Sha1 = 0x010000,
        Rsa2048Sha1 = 0x010001,
        EcdsaSha1 = 0x010002,
        Rsa4096Sha256 = 0x010003,
        Rsa2048Sha256 = 0x010004,
        EcdsaSha256 = 0x010005,
        HmacSha1 = 0x010006,
    };

    // We need to retrieve the encrypted title key for future decryption use
    Ticket::Ticket(const SysFs::FileBackingPtr& ticket) {
        const auto signatureType{ticket->Read<SignatureType>()};

        u64 offset{};

        // https://github.com/Thealexbarney/LibHac/blob/master/src/LibHac/Tools/Es/Ticket.cs#L62
        switch (signatureType) {
            case SignatureType::Rsa4096Sha1:
            case SignatureType::Rsa4096Sha256:
                offset += 0x23C;
                break;
            case SignatureType::Rsa2048Sha1:
            case SignatureType::Rsa2048Sha256:
                offset += 0x13C;
                break;
            case SignatureType::EcdsaSha1:
            case SignatureType::EcdsaSha256:
                offset += 0x7C;
                break;
            default: {
            }
        }

        offset = boost::alignment::align_up(offset, 0x40);
        assert(ticket->GetSize() - offset > sizeof(TicketData));

        ticket->Read(content, offset);
        assert(content.version == 2);

        std::print("Ticket issuer: {}\n", std::string_view(content.issuer.data()));
    }
    K128 Ticket::GetTitleKey() const {
        K128 title{};
        if (content.keyType == TicketKeyType::Common)
            std::memcpy(&title, &content.titleKeyBlock, title.size());

        return title;
    }
    std::pair<u64, K128> Ticket::GetRights() const {
        return std::make_pair(content.tickedId, content.rightsId);
    }
}