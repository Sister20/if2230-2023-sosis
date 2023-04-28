#include "lib-header/user-shell.h"

uint32_t cwdCluster = ROOT_CLUSTER_NUMBER;

void print(char* buf, uint8_t color) {
    syscall(5, (uint32_t) buf, strlen(buf), color);
}

void fgets(char* buf, tssize_t buf_size) {
    syscall(KEYBOARD_INPUT, (uint32_t) buf, buf_size, 0);
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
