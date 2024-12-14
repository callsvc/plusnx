#include <sys_fs/extra/nso_modules.h>

namespace Plusnx::SysFs::Extra {
    std::vector<FileBackingPtr> NsoModules::OrderExecutableFiles(const std::shared_ptr<Nx::PartitionFilesystem>& exefs) {
        std::vector<FileBackingPtr> files;

        assert(Nx::IsAExeFsPartition(exefs));
        const auto _modulesFiles{exefs->ListAllFiles()};

        // We need to maintain the order of these objects in memory layout
        std::vector<SysPath> modulesArray{"rtld", "main"};
        for (u32 sub{}; sub <= 6; sub++)
            modulesArray.emplace_back(std::format("subsdk{}", sub));
        modulesArray.emplace_back("sdk");

        for (const auto& target : modulesArray) {
            if (ContainsValue(_modulesFiles, target)) {
                files.emplace_back(exefs->OpenFile(target));
            }
        }

        return files;
    }

    void NsoModules::LoadModule(const std::shared_ptr<Nxk::Types::KProcess>& process, u64& address, const FileBackingPtr& file, bool allocate) {
        const auto loadAddr{address};

        const auto executable{std::make_shared<Nx::NsoCore>(file)};
        auto acceptArgs{file->path == "rtld"};
        if (!acceptArgs)
            if (executable->moduleName)
                acceptArgs = executable->moduleName->contains("rtld");

        executable->Load(process, address, acceptArgs, allocate);

        modules.insert_or_assign(loadAddr, std::move(executable));
    }

    std::pair<u64, u64> NsoModules::LoadProgramImage(const std::shared_ptr<Nxk::Types::KProcess>& process, u64& address, const std::vector<FileBackingPtr>& files, const bool allocate) {
        const u64 startAddress{address};

        const auto linkerAddress = [&] -> u64 {
            if (!modules.empty())
                return modules.lower_bound(startAddress)->first;
            return startAddress;
        }();

        if (allocate && modules.size()) {
            for (const auto& [loaderOffset, executable] : modules) {
                u64 ldrOffset{loaderOffset};
                executable->Load(process, ldrOffset, linkerAddress == ldrOffset);
                address = ldrOffset;
            }
        } else if (modules.empty()) {
            for (const auto& exec : files) {
                LoadModule(process, address, exec, allocate);
            }
        }

        return {linkerAddress, address - startAddress};
    }
}
