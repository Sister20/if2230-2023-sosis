#ifndef FS_SYSCALL_H
#define FS_SYSCALL_H
#include "stdtype.h"
#include "syscall.h"
#include "fat32.h"

tssize_t fs_read(struct FAT32DriverRequest request);

tssize_t fs_read_dir(struct FAT32DriverRequest request);

tssize_t fs_write(struct FAT32DriverRequest request);

tssize_t fs_delete(struct FAT32DriverRequest request);

#endif