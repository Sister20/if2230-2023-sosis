#include "lib-header/user-shell.h"

uint32_t cwdCluster = ROOT_CLUSTER_NUMBER;
char newline[2] = " \n";

void print(const char* buf, uint8_t color)
{
    syscall(TEXT_OUTPUT, (uint32_t) buf, strlen(buf), color);
}

void print_literal(const char *str, uint8_t color) {
    char temp[strlen(str) + 1];
    strcpy(temp, str);
    print(temp, color);
}

void printCWD(char *buf) {
    char root[15] = "User@Sosis:\\> ";
    print(root, 0x2);
    // print(buf, 0x2);
    if (buf) {

    }
    // print("> ", 0x2);
}

// void parseInt(int n, char *buf) {
//     int i = 0;
//     int j = 0;
//     char temp;

//     if (n == 0) {
//         buf[i++] = '0';
//         buf[i] = '\0';
//         return;
//     }

//     while (n) {
//         buf[i++] = (n % 10) + '0';
//         n = n / 10;
//     }

//     buf[i] = '\0';

//     i--;
//     while (j < i) {
//         temp = buf[j];
//         buf[j] = buf[i];
//         buf[i] = temp;
//         j++;
//         i--;
//     }
// }

void log(char* buf) {
    syscall(5, (uint32_t)buf, strlen(buf), 0xF);
}

void fgets(char *buf, tssize_t buf_size)
{
    syscall(KEYBOARD_INPUT, (uint32_t) buf, buf_size, 0);
}

void commandParser(char* buf)
{
    int space_index = 0;
    bool valid = FALSE;
    char text[13] = "inside here\n";
    log(text);
    // char tex2[] = "inside here2\n";
    // log(tex2);
    if (strlen(buf) == 2)
    {
        char text[11] = "inside if\n";
        log(text);
        // for ls command
        space_index = 2;
        valid = TRUE;
    }
    else
    {
        char text[13] = "inside else\n";
        log(text);
        for (int i = 0; i < strlen(buf); i++)
        {
            char text[6] = "loop\n";
            log(text);
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
        // char text[] = "inside switch\n";
        // log(text);
        switch (space_index)
        {
        case 2:
        {
            char text[6] = "case\n";
            log(text);
            // process command with 2 char long
            char two_char_cmd[3];
            two_char_cmd[0] = buf[0];
            two_char_cmd[1] = buf[1];
            two_char_cmd[2] = '\0';
            int offset = 3;
            log(two_char_cmd);
            if (strcmp(two_char_cmd, "cd") == 0)
            {
                // char inside_if[4] = "if\n";
                // log(inside_if);
                int pathLength = strlen(buf) - offset; // remove newline
                char path[pathLength + 1];
                for (int i = 0; i < pathLength; i++)
                {
                    char text[6] = "loop\n";
                    log(text);
                    path[i] = buf[i + offset];
                }
                path[pathLength] = '\0';
                // log(path);
                // int8_t retcode = cd(&cwdCluster, path);
                // if (retcode == 0) {
                //     char success[10] = "Success!\n";
                //     print(success, 0xF);
                // } else {
                //     char failed[7] = "Fail!\n";
                //     print(failed, 0xF);
                // }
                // char text[20] = "Caught command: cd\n";
                // print(text, 0xF);
                // char text1[20] = "Caught path: ";
                // print(text1, 0xF);
                // print(path, 0xF);
                // print(newline, 0xF);
            }
            else if (strcmp(two_char_cmd, "ls") == 0)
            {
                // ls(cwdCluster);
                print("Caught command: ls\n", 0xF);
            }
            else if (strcmp(two_char_cmd, "cp\0") == 0)
            {
                print("Caught command: cp\n", 0xF);
            }
            else if (strcmp(two_char_cmd, "rm\0") == 0)
            {
                print("Caught command: rm\n", 0xF);
            }
            else if (strcmp(two_char_cmd, "mv\0") == 0)
            {
                print("Caught command: mv\n", 0xF);
            } else {
                char text[9] = "on else\n";
                log(text);
            }
            break;
        }
        default:
            
            break;
        }
    } else {
        print("Invalid command ", 0xF);
        // print(buf, 0xF);
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
    syscall(0, (uint32_t) &request, (uint32_t) &retcode, 0);
    if (retcode == 0)
        syscall(5, (uint32_t) "owo\n", 4, 0xF);

    char buf[16];
    while (TRUE) {
        // print("User@localhost\\> \0", 0xF);
        printCWD(buf);
        fgets(buf, 16);
        commandParser(buf);
        // syscall(4, (uint32_t) buf, 16, 0);
        // syscall(5, (uint32_t) buf, 16, 0xF);
    }

    return 0;
}

