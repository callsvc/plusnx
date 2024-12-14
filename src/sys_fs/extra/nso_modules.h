#pragma once

#include <sys_fs/nx/partition_filesystem.h>
#include <sys_fs/nx/nso_core.h>
namespace Plusnx::SysFs::Extra {
    class NsoModules {
    public:
        NsoModules() = default;

        static std::vector<FileBackingPtr> OrderExecutableFiles(const std::shared_ptr<Nx::PartitionFilesystem>& exefs);
        void LoadModule(const std::shared_ptr<Nxk::Types::KProcess>& process, u64& address, const FileBackingPtr& file, bool allocate);
        std::pair<u64, u64> LoadProgramImage(const std::shared_ptr<Nxk::Types::KProcess>& process, u64& address, const std::vector<FileBackingPtr>& files, bool allocate = true);

        std::map<u64, std::shared_ptr<Nx::NsoCore>> modules;
    };
}