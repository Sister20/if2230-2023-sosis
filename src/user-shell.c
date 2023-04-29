#include "lib-header/user-shell.h"

struct CWDdata cwd_data = {
    .currentCluster = ROOT_CLUSTER_NUMBER,
    .prevCluster = ROOT_CLUSTER_NUMBER,
    .cwdName = {"root\0\0\0\0"}};

void fgets(char *buf, tssize_t buf_size)
{
    syscall(KEYBOARD_INPUT, (uint32_t)buf, buf_size, 0);
}

void printCWD() {
    print("User@Sosis:\\", 0x2);
    if (strcmp(cwd_data.cwdName, "root\0\0\0\0") != 0) {
        print(cwd_data.cwdName, 0x2);
    }
    print("> ", 0x2);
}

void commandParser(char *buf)
{
    int space_index = 0;
    bool valid = FALSE;
    if (strlen(buf) == 2)
    {
        // for ls command
        space_index = 2;
        valid = TRUE;
    }
    else if (strlen(buf) > 25)
    {
        print("Please check your input!\n", 0x4);
        return;
    }
    else
    {
        int buf_len = strlen(buf);
        for (int i = 0; i < buf_len; i++)
        {
            if (buf[i] == ' ' && !valid)
            {
                space_index = i;
                valid = TRUE;
                break;
            }
        }
    }
    if (valid)
    {
        if (space_index == 2)
        {
            // process command with 2 char long
            int offset = 3;
            char two_char_cmd[offset];
            char args[strlen(buf) - offset + 1];
            split(buf, two_char_cmd, args, offset);
            if (strcmp(two_char_cmd, "cd\0") == 0)
            {
                cd(&cwd_data, args);
            }
            else if (strcmp(two_char_cmd, "ls") == 0)
            {
                ls(cwd_data);
            }
            else if (strcmp(two_char_cmd, "cp") == 0)
            {
                print("Caught command: cp\n", 0xF);
                int count = 0;
                for(int i = 0; i< strlen(args); i++){
                    if(args[i] == ' '){
                        break;
                    }
                    count++;
                }
                int secOffset = count+1;
                char first_string_arg[secOffset];
                char second_string_arg[strlen(args) - secOffset + 1];
                split(args, first_string_arg, second_string_arg, secOffset);
                int retcode = cp(cwd_data.currentCluster, first_string_arg, second_string_arg);
                if (retcode == 0) {
                    char success[10] = "Success!\n";
                    print(success, 0xF);
                } else {
                    char failed[7] = "Fail!\n";
                    print(failed, 0xF);
                }
            }
            else if (strcmp(two_char_cmd, "rm") == 0)
            {
                print("Caught command: rm\n", 0xF);
                int retcode = rm(cwd_data.currentCluster, args);
                if (retcode == 0) {
                    char success[10] = "Success!\n";
                    print(success, 0xF);
                } else {
                    char failed[7] = "Fail!\n";
                    print(failed, 0xF);
                }
            }
            else if (strcmp(two_char_cmd, "mv") == 0)
            {
                print("Caught command: mv\n", 0xF);
            }
            else
            {
                log("on else\n");
            }
        }
        else if (space_index == 3)
        {
            // cat command : cat filename
            int offset = 4;
            char three_char_cmd[offset];
            char filename[strlen(buf) - offset + 1];
            split(buf, three_char_cmd, filename, offset);
            if (strcmp(three_char_cmd, "cat\0") == 0)
            {
                print("Caught command: cat\n", 0xF);
                print("Caught filename: ", 0xF);
                print(filename, 0xF);
                print("\n", 0xF);
            }
        }
        else if (space_index == 5)
        {
            // mkdir command : mkdir dirname
            int offset = 6;
            char five_char_cmd[offset];
            char dirname[strlen(buf) - offset + 1];
            split(buf, five_char_cmd, dirname, offset);
            if (strcmp(five_char_cmd, "mkdir\0") == 0)
            {
                // print("Caught command: mkdir\n", 0xF);
                // print("Caught dirname: ", 0xF);
                // print(dirname, 0xF);
                // print("\n", 0xF);
                mkdir(cwd_data.currentCluster, dirname);
            }
        }
        else if (space_index == 7)
        {
            // whereis command : whereis name
            int offset = 8;
            char seven_char_cmd[offset];
            char name[strlen(buf) - offset + 1];
            split(buf, seven_char_cmd, name, offset);
            if (strcmp(seven_char_cmd, "whereis\0") == 0)
            {
                print("Caught command: whereis\n", 0xF);
                print("Caught name: ", 0xF);
                log(name);
                print("\n", 0xF);
            }
        }
        else
        {
            print("Command ", 0xF);
            print(buf, 0xF);
            print(" not found\n", 0xF);
        }
    }
    else
    {
        print("Invalid command ", 0xF);
        print(buf, 0xF);
        print("!\n", 0xF);
    }
}

int main(void)
{
    struct ClusterBuffer cl = {0};
    // struct FAT32DriverRequest request = {
    //     .buf                   = &cl,
    //     .name                  = "ikanaido",
    //     .ext                   = "\0\0\0",
    //     .parent_cluster_number = ROOT_CLUSTER_NUMBER,
    //     .buffer_size           = CLUSTER_SIZE,
    // };
    struct FAT32DriverRequest request = {
        .buf = &cl,
        .name = "ikanaide",
        .ext = "\0\0\0",
        .parent_cluster_number = ROOT_CLUSTER_NUMBER,
        .buffer_size = 0,
    };

    // write(request);  // Create folder "ikanaide"
    int32_t retcode;
    syscall(FS_WRITE, (uint32_t)&request, (uint32_t)&retcode, 0);
    if (retcode == 0)
    {
        syscall(5, (uint32_t) "owo\n", 4, 0xF);
    }

    char buf[26];
    while (TRUE)
    {
        printCWD();
        fgets(buf, 26);
        // parse buf here
        commandParser(buf);
    }

    return 0;
}
