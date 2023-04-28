#include "lib-header/user-shell.h"

uint32_t cwdCluster = ROOT_CLUSTER_NUMBER;

void print(char* buf, uint8_t color) {
    syscall(5, (uint32_t) buf, strlen(buf), color);
}

void fgets(char* buf, tssize_t buf_size) {
    syscall(KEYBOARD_INPUT, (uint32_t) buf, buf_size, 0);
}

void log(char* buf) {
    syscall(5, (uint32_t)buf, strlen(buf), 0xF);
}

void commandParser(char* buf)
{
    int space_index = 0;
    bool valid = FALSE;
    if (strlen(buf) == 2)
    {
        log("Caught input length <= 2\n");
        // for ls command
        space_index = 2;
        valid = TRUE;
    }
    else
    {
        log("Caught input length > 2\n");
        for (int i = 0; i < strlen(buf); i++)
        {
            // log("loop\n");
            if (buf[i] == ' ' && !valid)
            {
                space_index = i;
                valid = TRUE;
            }
            else if (buf[i] == ' ' && valid)
            {
                valid = FALSE;
                break;
            }
        }
    }
    if (valid)
    {
        switch (space_index)
        {
        case 2:
        {
            // process command with 2 char long
            char two_char_cmd[3];
            two_char_cmd[0] = buf[0];
            two_char_cmd[1] = buf[1];
            two_char_cmd[2] = '\0';
            int offset = 3;
            if (strcmp(two_char_cmd, "cd") == 0)
            {
                int dirLength = strlen(buf) - offset; // remove newline
                char dir[dirLength + 1];
                for (int i = 0; i < dirLength; i++)
                {
                    dir[i] = buf[i + offset];
                }
                dir[dirLength] = '\0';
                // log(path);
                // int8_t retcode = cd(&cwdCluster, path);
                // if (retcode == 0) {
                //     char success[10] = "Success!\n";
                //     print(success, 0xF);
                // } else {
                //     char failed[7] = "Fail!\n";
                //     print(failed, 0xF);
                // }
                print("Caught command: cd\n", 0xF);
                print("Caught path: ", 0xF);
                print(dir, 0xF);
                print("\n", 0xF);
            }
            else if (strcmp(two_char_cmd, "ls") == 0)
            {
                // ls(cwdCluster);
                print("Caught command: ls\n", 0xF);
            }
            else if (strcmp(two_char_cmd, "cp") == 0)
            {
                print("Caught command: cp\n", 0xF);
            }
            else if (strcmp(two_char_cmd, "rm") == 0)
            {
                print("Caught command: rm\n", 0xF);
            }
            else if (strcmp(two_char_cmd, "mv") == 0)
            {
                print("Caught command: mv\n", 0xF);
            } else {
                log("on else\n");
            }
            break;
        }
        case 3: {
            // cat command : cat filename
            char three_char_cmd[4];
            three_char_cmd[0] = buf[0];
            three_char_cmd[1] = buf[1];
            three_char_cmd[1] = buf[2];
            three_char_cmd[3] = '\0';
            int offset = 4;
            if (strcmp(three_char_cmd, "cat") == 0) {
                int filenameLength = strlen(buf) - offset; // remove newline
                char filename[filenameLength + 1];
                for (int i = 0; i < filenameLength; i++)
                {
                    filename[i] = buf[i + offset];
                }
                filename[filenameLength] = '\0';
                print("Caught command: cat\n", 0xF);
                print("Caught filename: ", 0xF);
                print(filename, 0xF);
                print("\n", 0xF);
            }
            break;
        }
        default:
            break;
        }
    } else {
        print("Invalid command ", 0xF);
        print(buf, 0xF);
        print("!\n", 0xF);
    }
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
        fgets(buf, 16);
        commandParser(buf);
        // parse buf here

    }

    return 0;
}
