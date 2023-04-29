#include "lib-header/fs-syscall.h"

tssize_t fs_read(struct FAT32DriverRequest request)
{
    int8_t retcode = 99;
    syscall(FS_READ, (uint32_t) &request, (uint32_t)&retcode, 0);
    return (retcode);
}

tssize_t fs_read_dir(struct FAT32DriverRequest request)
{
    int8_t retcode = 99;
    syscall(FS_READ_DIR, (uint32_t) &request, (uint32_t)&retcode, 0);
    return (retcode);
}

tssize_t fs_write(struct FAT32DriverRequest request)
{
    int8_t retcode = 99;
    syscall(FS_WRITE, (uint32_t) &request, (uint32_t)&retcode, 0);
    return (retcode);
}

tssize_t fs_delete(struct FAT32DriverRequest request)
{
    int8_t retcode = 99;
    syscall(FS_DELETE, (uint32_t) &request, (uint32_t)&retcode, 0);
    return (retcode);
}
