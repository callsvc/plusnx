#include <print>

#include <sys_fs/continuous_block.h>
#include <sys_fs/npdm.h>

namespace Plusnx::SysFs {
    Npdm::Npdm(const FileBackingPtr& npdm) :
        infos(std::make_unique<ContinuousBlock>(npdm)) {

        if (infos->Read(content) != sizeof(content))
            throw Except("Cannot read NPDM content");

        assert(content.magic == ConstMagic<u32>("META"));
        if (std::string_view(content.name) == "Application") {}

        if (infos->Read(acid) != sizeof(acid)) {
            return;
        }
        assert(acid.magic == ConstMagic<u32>("ACID"));
        infos->SkipBytes(content.acidSize - sizeof(AcidHeader));
        if (const auto cursor = infos->GetCursor(); cursor != content.aciOffset) {
            infos->SkipBytes(content.aciOffset - cursor);
        }
        if (infos->Read(aci0) != sizeof(aci0)) {
            return;
        }
        assert(aci0.magic == ConstMagic<u32>("ACI0"));

        environment = content.flags.addressSpace;
    }

    void Npdm::DisplayBinaryInformation() const {
        std::stringstream info;
        info << std::format("Used address space: {} Bits\n", GetHostBitsCount(environment));

        std::print("{}", info.str());
    }
}
