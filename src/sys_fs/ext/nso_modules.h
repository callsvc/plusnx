#pragma once

#include <sys_fs/nx/nso_core.h>
namespace Plusnx::SysFs::Ext {
    class NsoModules {
    public:
        NsoModules() = default;

        void LoadModule(const std::shared_ptr<GenericKernel::Types::KProcess>& process, u64& address, const FileBackingPtr& file);
        void LoadProgramImage(const std::shared_ptr<GenericKernel::Types::KProcess>& process, u64& address, const std::vector<FileBackingPtr>& files);

        std::map<u64, std::shared_ptr<Nx::NsoCore>> modules;
    };
}