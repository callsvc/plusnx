#include <loader/cartridge.h>

#include <sys_fs/nx/partition_filesystem.h>
#include <sys_fs/layered_fs.h>
namespace Plusnx::Loader {
    Cartridge::Cartridge(const SysFs::FileBackingPtr& xci) : AppLoader(AppType::Xci) {
        assert(xci->GetSize() > sizeof(content));
        if (xci->Read(content) != sizeof(content))
            return;

        const auto &header{content.cardHeader};
        assert(header.magic == ConstMagic<u32>("HEAD"));
        assert(header.backupStartAddr == 0xFFFFFFFF);

        auto partition{std::make_unique<SysFs::FileLayered>(xci, "", header.pfsBeginAddr, header.pfsSize)};
        pfs = std::make_unique<SysFs::Nx::PartitionFilesystem>(std::move(partition));

    }
    void Cartridge::Load(std::shared_ptr<Core::Context>& context) {
    }
}
