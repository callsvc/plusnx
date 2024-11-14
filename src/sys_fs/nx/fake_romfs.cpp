
#include <sys_fs/nx/fake_romfs.h>
Plusnx::SysFs::Nx::FakeRomFs::FakeRomFs() {
    AddDirectory({});
}
void Plusnx::SysFs::Nx::FakeRomFs::AddFile(const FileBackingPtr& file) {
    assert(file->path.is_absolute());
    ReadOnlyFilesystem::AddFile(file->path, file);
}
