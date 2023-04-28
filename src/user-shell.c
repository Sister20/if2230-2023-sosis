#include "lib-header/user-shell.h"

uint32_t cwdCluster = ROOT_CLUSTER_NUMBER;

void syscall(uint32_t eax, uint32_t ebx, uint32_t ecx, uint32_t edx) {
    __asm__ volatile("mov %0, %%ebx" : /* <Empty> */ : "r"(ebx));
    __asm__ volatile("mov %0, %%ecx" : /* <Empty> */ : "r"(ecx));
    __asm__ volatile("mov %0, %%edx" : /* <Empty> */ : "r"(edx));
    __asm__ volatile("mov %0, %%eax" : /* <Empty> */ : "r"(eax));
    // Note : gcc usually use %eax as intermediate register,
    //        so it need to be the last one to mov
    __asm__ volatile("int $0x30");
}

void print(char* buf, uint8_t color) {
    syscall(5, (uint32_t) buf, strlen(buf), color);
}

void fgets(char* buf, tssize_t buf_size) {
    syscall(KEYBOARD_INPUT, (uint32_t) buf, buf_size, 0);
}

tssize_t fs_read(struct FAT32DriverRequest* request) {
    int8_t retcode = 99;
    syscall(FS_READ, (uint32_t) request, (uint32_t) &retcode, 0);
    return (retcode);
}

tssize_t fs_read_dir(struct FAT32DriverRequest* request) {
    int8_t retcode = 99;
    syscall(FS_READ_DIR, (uint32_t) request, (uint32_t) &retcode, 0);
    return (retcode);
}

tssize_t fs_write(struct FAT32DriverRequest* request) {
    int8_t retcode = 99;
    syscall(FS_WRITE, (uint32_t) request, (uint32_t) &retcode, 0);
    return (retcode);
}

tssize_t fs_delete(struct FAT32DriverRequest* request) {
    int8_t retcode = 99;
    syscall(FS_DELETE, (uint32_t) request, (uint32_t) &retcode, 0);
    return (retcode);
}



int main(void) {
    struct ClusterBuffer cl           = {0};
    struct FAT32DriverRequest request = {
        .buf                   = &cl,
        .name                  = "ikanaide",
        .ext                   = "\0\0\0",
        .parent_cluster_number = ROOT_CLUSTER_NUMBER,
        .buffer_size           = CLUSTER_SIZE,
    };
    int32_t retcode;
    syscall(FS_WRITE, (uint32_t) &request, (uint32_t) &retcode, 0);
    if (retcode == 0) {
        syscall(5, (uint32_t) "owo\n", 4, 0xF);
    }
    
    char buf[16];
    while (TRUE) {
        print("User@Sosis:\\> ", 0x2);
        fgets(buf, strlen(buf));

        // parse buf here

    }

    return 0;
}
