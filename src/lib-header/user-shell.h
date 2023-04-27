#ifndef USER_SHELL_H
#define USER_SHELL_H

#include "stdtype.h"
#include "fat32.h"

#define FS_READ 0
#define FS_READ_DIR 1
#define FS_WRITE 2
#define FS_DELETE 3
#define KEYBOARD_INPUT 4
#define TEXT_OUTPUT 5

/**
 * Get character from keyboard input 
*/
void fgets(char* buf, tssize_t buf_size);

/**
 * 
*/
tssize_t fs_read(struct FAT32DriverRequest* request);

tssize_t fs_read_dir(struct FAT32DriverRequest* request);

tssize_t fs_write(struct FAT32DriverRequest* request);

tssize_t fs_delete(struct FAT32DriverRequest* request);

#endif