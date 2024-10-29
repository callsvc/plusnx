#include <print>
#include <sys_fs/npdm.h>

namespace Plusnx::SysFs {
    Npdm::Npdm(const FileBackingPtr& npdm) {
        if (npdm->Read(content) != sizeof(content)) {
            throw std::runtime_error("Cannot read NPDM content");
        }

        assert(content.magic == ConstMagic<u32>("META"));
        assert(std::string_view(content.name) == "Application");

        environment = content.flags.processAs;
    }

    void Npdm::DisplayBinaryInformation() const {
        std::stringstream info;
        info << std::format("Used address space: {}\n", GetHostBitsCount(environment));

        std::print("{}\n", info.str());
    }
}
