

#include <sys_fs/bounded.h>
Plusnx::SysFs::FileBounded::FileBounded(const FileBackingPtr& backing, const SysPath& name, const u64 base, const u64 bound) :
    FileBacking(name), parent(backing), starts(base), ends(bound) {}
Plusnx::u64 Plusnx::SysFs::FileBounded::GetSize() const {
    assert(parent->GetSize() >= ends);
    return ends;
}
Plusnx::u64 Plusnx::SysFs::FileBounded::ReadImpl(void* output, const u64 size, const u64 offset) {
    if (size > ends)
        throw std::runtime_error("Out of the current file bounds");

    const auto result{parent->Read(output, size, starts + offset)};
    return result;
}
