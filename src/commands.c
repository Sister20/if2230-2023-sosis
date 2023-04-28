#include "lib-header/commands.h"



void cd(uint32_t clusterNumber, char* path) {

}

void ls(uint32_t clusterNumber) {
    
}

void mkdir(uint32_t clusterNumber, char* dirName) {
    struct FAT32DriverRequest request = {
        .buf                   = 0,
        .name                  = dirName,
        .ext                   = "\0\0\0",
        .parent_cluster_number = clusterNumber,
        .buffer_size           = 0,
    };
    int32_t retcode;
    syscall(FS_WRITE, (uint32_t) &request, (uint32_t) &retcode, 0);

    switch (retcode) {
        case 0: {
            syscall(TEXT_OUTPUT, (uint32_t) "mkdir: File created\n", 20, 0xF);
            break;
        }
        case 1: {
            syscall(TEXT_OUTPUT, (uint32_t) "mkdir: cannot create directory: File exists\n", 44, 0xF);
            break;
        }
        case 2: {
            syscall(TEXT_OUTPUT, (uint32_t) "mkdir: cannot create directory: Invalid parent cluster\n", 55, 0xF);
            break;
        }
        default: {
            syscall(TEXT_OUTPUT, (uint32_t) "mkdir: Unknown error\n", 21, 0xF);
            break;
        }
    }
}

void cat(uint32_t clusterNumber, char* fileName, char* fileExt) {
    struct ClusterBuffer cbuf[4];
    struct FAT32DriverRequest request = {
        .buf                   = cbuf,
        .name                  = fileName,
        .ext                   = fileExt,
        .parent_cluster_number = clusterNumber,
        .buffer_size           = 4 * CLUSTER_SIZE,
    };
    int32_t retcode;
    syscall(FS_READ, (uint32_t) &request, (uint32_t) &retcode, 0);

    switch (retcode) {
        case 0: {
            syscall(TEXT_OUTPUT, (uint32_t) "cat: File read\n", 15, 0xF);
            // chore: output file contents
            break;
        }
        case 1: {
            syscall(TEXT_OUTPUT, (uint32_t) "cat: Not a file\n", 16, 0xF);
            break;
        }
        case 2: {
            syscall(TEXT_OUTPUT, (uint32_t) "cat: Exceeded buffer\n", 21, 0xF);
            break;
        }
        case 3: {
            syscall(TEXT_OUTPUT, (uint32_t) "cat: File not found\n", 20, 0xF);
            break;
        }
        default: {
            syscall(TEXT_OUTPUT, (uint32_t) "cat: Unknown error\n", 19, 0xF);
            break;
        }
    }
}
