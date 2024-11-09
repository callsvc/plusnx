

#include <sys_fs/layered_fs.h>
Plusnx::SysFs::FileLayered::FileLayered(const FileBackingPtr& backing, const SysPath& name, const u64 base, const u64 bound) :
    FileBacking(name), parent(backing), starts(base), ends(bound) {}
Plusnx::u64 Plusnx::SysFs::FileLayered::GetSize() const {
    assert(parent->GetSize() >= ends);
    return ends;
}
Plusnx::u64 Plusnx::SysFs::FileLayered::ReadImpl(void* output, const u64 size, const u64 offset) {
    if (offset + size > ends)
        throw Except("Out of the current file bounds");

    const auto result{parent->Read(output, size, starts + offset)};
    return result;
}

Plusnx::u64 Plusnx::SysFs::FileLayered::WriteImpl([[maybe_unused]] const void* input, [[maybe_unused]] u64 size, [[maybe_unused]] u64 offset) {
    throw Except("It is not possible to write data to virtual files");
}
