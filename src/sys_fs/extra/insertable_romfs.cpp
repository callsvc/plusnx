
#include <sys_fs/extra/insertable_romfs.h>
Plusnx::SysFs::Extra::InsertableRomFs::InsertableRomFs() {
    AddDirectory({});
}
void Plusnx::SysFs::Extra::InsertableRomFs::AddFile(const FileBackingPtr& file) {
    assert(file->path.is_absolute());
    ReadOnlyFilesystem::AddFile(file->path, file);
}
