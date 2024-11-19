
#include <sys_fs/ext/fake_romfs.h>
Plusnx::SysFs::Ext::FakeRomFs::FakeRomFs() {
    AddDirectory({});
}
void Plusnx::SysFs::Ext::FakeRomFs::AddFile(const FileBackingPtr& file) {
    assert(file->path.is_absolute());
    ReadOnlyFilesystem::AddFile(file->path, file);
}
