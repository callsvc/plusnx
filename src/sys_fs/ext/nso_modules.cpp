#include <sys_fs/ext/nso_modules.h>

namespace Plusnx::SysFs::Ext {
    void NsoModules::LoadModule(const std::shared_ptr<GenericKernel::Types::KProcess>& process, u64& address, const FileBackingPtr& file) {
        const auto loadAddr{address};

        const auto executable{std::make_shared<Nx::NsoCore>(file)};
        auto acceptArgs{file->path == "rtld"};
        if (!acceptArgs)
            if (executable->moduleName)
                acceptArgs = executable->moduleName->contains("rtld");
        executable->Load(process, address, acceptArgs);

        modules.insert_or_assign(loadAddr, std::move(executable));
    }

    void NsoModules::LoadProgramImage(const std::shared_ptr<GenericKernel::Types::KProcess>& process, u64& address, const std::vector<FileBackingPtr>& files) {
        for (const auto& exec : files)
            LoadModule(process, address, exec);
    }
}
