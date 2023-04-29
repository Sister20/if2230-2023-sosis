#include "lib-header/commands.h"

void print(char *buf, uint8_t color)
{
    syscall(5, (uint32_t)buf, strlen(buf), color);
}

void log(char *buf)
{
    syscall(5, (uint32_t)buf, strlen(buf), 0xF);
}

void cd(struct CWDdata *cwd_data, char *folderName)
{
    // struct ClusterBuffer cl = {0};
    struct FAT32DirectoryTable dt = {0};
    char name[8];
    if (strlen(folderName) <= 8)
    {
        int i = 0;
        while (i < 8)
        {
            if (i < strlen(folderName))
            {
                name[i] = folderName[i];
            }
            else
            {
                name[i] = '\0';
            }
            i++;
        }
    }
    else
    {
        // return -1;
        log("Wrong folder name!\n");
    }
    // log(folderName);
    // log("\n");
    char back_route[3] = "..";
    if (strcmp(folderName, back_route) == 0)
    {
        if (cwd_data->currentCluster == ROOT_CLUSTER_NUMBER)
        {
            return;
        }
        else
        {
            // move back to its parent
            struct FAT32DriverRequest request = {
                .buf = &dt,
                .ext = {0},
                .name = {0},
                .parent_cluster_number = cwd_data->prevCluster,
                .buffer_size = CLUSTER_SIZE,
            };
            strncpy(request.name, cwd_data->cwdName, 8);
            strncpy(request.ext, "\0\0\0", 3);
            int8_t retcode = fs_read_dir(request);
            if (retcode == 0)
            {
                // bool found = FALSE;
                // int index = 0;
                struct FAT32DirectoryTable *directory_table = (struct FAT32DirectoryTable *)request.buf;
                // while (!found)
                // {
                struct FAT32DirectoryEntry entry = directory_table->table[0];
                // if (strcmp(entry.name, request.name) == 0 && entry.attribute == ATTR_SUBDIRECTORY)
                // {
                    // search that folder here then get the cluster number
                    cwd_data->currentCluster = (((uint32_t)entry.cluster_high << 16) | entry.cluster_low);
                    strncpy(cwd_data->cwdName, entry.name, 8);
                    cwd_data->prevCluster = ROOT_CLUSTER_NUMBER;
                    // break;
                // }
                    // index++;
                // }
                // log("Success !\n");
            }
            else
            {
                log("Fail !\n");
            }
        }
    }
    else
    {
        struct FAT32DriverRequest request = {
            .buf = &dt,
            .ext = {0},
            .name = {0},
            .parent_cluster_number = cwd_data->currentCluster,
            .buffer_size = CLUSTER_SIZE,
        };
        strncpy(request.name, name, 8);
        strncpy(request.ext, "\0\0\0", 3);
        int8_t retcode = fs_read_dir(request);
        if (retcode == 0)
        {
            bool found = FALSE;
            int index = 0;
            struct FAT32DirectoryTable *directory_table = (struct FAT32DirectoryTable *)request.buf;
            while (!found)
            {
                struct FAT32DirectoryEntry entry = directory_table->table[index];
                if (strcmp(entry.name, request.name) == 0 && entry.attribute == ATTR_SUBDIRECTORY)
                {
                    // search that folder here then get the cluster number
                    cwd_data->prevCluster = cwd_data->currentCluster;
                    cwd_data->currentCluster = (((uint32_t)entry.cluster_high << 16) | entry.cluster_low);
                    // if (strcmp(cwd_data->cwdName, "\0\0\0\0\0\0\0\0") == 0 || strcmp(cwd_data->cwdName, "root\0\0\0\0") == 0)
                    // {
                    strncpy(cwd_data->cwdName, entry.name, 8);
                    // }
                    // else
                    // {
                    //     strcat(cwd_data->cwdName, "\\");
                    //     strcat(cwd_data->cwdName, entry.name);
                    // }
                    found = TRUE;
                }
                index++;
            }
            // log("Success !\n");
        }
        else
        {
            log("No Such Folder !\n");
        }
    }
    memset(&dt, 0, sizeof(dt));
}

void ls(struct CWDdata cwd_data)
{
    struct FAT32DirectoryTable dt = {0};
    struct FAT32DriverRequest request = {
        .buf = &dt,
        .ext = {0},
        .name = {0},
        .parent_cluster_number = cwd_data.currentCluster,
        .buffer_size = CLUSTER_SIZE,
    };
    strncpy(request.name, cwd_data.cwdName, 8);
    strncpy(request.ext, "\0\0\0", 3);
    int8_t retcode = fs_read_dir(request);
    if (retcode == 0)
    {
        int index = 0;
        struct FAT32DirectoryTable *directory_table = (struct FAT32DirectoryTable *)request.buf;
        while (directory_table->table[index].name[0] != '\0')
        {
            struct FAT32DirectoryEntry entry = directory_table->table[index];
            char filesize[100];
            parse_int(entry.filesize, filesize);
            log("Name           Extension           Filesize(Bytes)\n");
            log(entry.name);
            int name_len = strlen(entry.name);
            for (int i = 0; i < 8 - name_len; i++) {
                log(" ");
            }
            log("        ");
            int ext_len = strlen(entry.ext);
            if (entry.ext[0] != '\0') {
                log(entry.ext);
                for (int i = 0; i < 3 - ext_len; i++) {
                    log(" ");
                }
            }
            else {
                for (int i = 0; i < 3; i++) {
                    log("-");
                }
            }
            log("                ");
            if (entry.filesize != 0) {
                log(filesize);
            } else {
                log("0");
            }
            log("\n");
            index++;
        }
        log("\n");
    }
    memset(&dt, 0, sizeof(dt));
}

void mkdir(uint32_t clusterNumber, char *dirName)
{
    struct FAT32DriverRequest request = {
        .buf = 0,
        .name = {0},
        .ext = "\0\0\0",
        .parent_cluster_number = clusterNumber,
        .buffer_size = 0,
    };
    strcpy(request.name, dirName);
    int32_t retcode;
    syscall(FS_WRITE, (uint32_t)&request, (uint32_t)&retcode, 0);

    switch (retcode)
    {
    case 0:
    {
        syscall(TEXT_OUTPUT, (uint32_t) "mkdir: File created\n", 20, 0xF);
        break;
    }
    case 1:
    {
        syscall(TEXT_OUTPUT, (uint32_t) "mkdir: cannot create directory: File exists\n", 44, 0xF);
        break;
    }
    case 2:
    {
        syscall(TEXT_OUTPUT, (uint32_t) "mkdir: cannot create directory: Invalid parent cluster\n", 55, 0xF);
        break;
    }
    default:
    {
        syscall(TEXT_OUTPUT, (uint32_t) "mkdir: Unknown error\n", 21, 0xF);
        break;
    }
    }
}

void cat(uint32_t clusterNumber, char *fileName)
{
    struct ClusterBuffer cbuf[4];
    struct FAT32DriverRequest request = {
        .buf = cbuf,
        .name = {0},
        .ext = {0},
        .parent_cluster_number = clusterNumber,
        .buffer_size = 4 * CLUSTER_SIZE,
    };
    strncpy(request.name, fileName, 8);
    int32_t retcode;
    syscall(FS_READ, (uint32_t)&request, (uint32_t)&retcode, 0);

    switch (retcode)
    {
    case 0:
    {
        syscall(TEXT_OUTPUT, (uint32_t) "cat: File read\n", 15, 0xF);

        // log(cbuf); chore: fix
        break;
    }
    case 1:
    {
        syscall(TEXT_OUTPUT, (uint32_t) "cat: Not a file\n", 16, 0xF);
        break;
    }
    case 2:
    {
        syscall(TEXT_OUTPUT, (uint32_t) "cat: Exceeded buffer\n", 21, 0xF);
        break;
    }
    case 3:
    {
        syscall(TEXT_OUTPUT, (uint32_t) "cat: File not found\n", 20, 0xF);
        break;
    }
    default:
    {
        syscall(TEXT_OUTPUT, (uint32_t) "cat: Unknown error\n", 19, 0xF);
        break;
    }
    }
}

int cp(struct CWDdata cwd_data, char *sourceFileName, char *destinationFileName)
{
    struct FAT32DirectoryTable dummy_read_dir_request;
    memset(&dummy_read_dir_request, 0, sizeof(dummy_read_dir_request));
    struct FAT32DriverRequest dirRequest = {
        .buf = &dummy_read_dir_request,
        .name = {0},
        .ext = {0},
        .parent_cluster_number = cwd_data.currentCluster,
        .buffer_size = CLUSTER_SIZE,
    };

    strncpy(dirRequest.name, cwd_data.cwdName, 8);
    strncpy(dirRequest.ext, "\0\0\0", 3);

    tssize_t dirReadResult = fs_read_dir(dirRequest);

    if (dirReadResult != 0)
    {
        return 1;
    }

    char ext[3];
    uint32_t filesize;
    struct FAT32DirectoryTable *dirtable = (struct FAT32DirectoryTable *)dirRequest.buf;
    int getData = -1;
    for (tssize_t i = 0; i < CLUSTER_SIZE / sizeof(struct FAT32DirectoryEntry); i++)
    {
        struct FAT32DirectoryEntry entry = dirtable->table[i];
        if (entry.name[0] == '\0')
        {
            break;
        }
        if (entry.attribute != ATTR_SUBDIRECTORY &&
            strcmp(entry.name, sourceFileName) == 0)
        {
            strncpy(ext, entry.ext, 3);
            filesize = entry.filesize;
            getData = 1;
            break;
        }
    }

    if (getData == -1)
    {
        return 2;
    }

    struct ClusterBuffer dummy_read_request[filesize/sizeof(struct ClusterBuffer)];
    struct FAT32DriverRequest readRequest = {
        .buf = &dummy_read_request,
        .name = {0},
        .ext = {0},
        .parent_cluster_number = cwd_data.currentCluster,
        .buffer_size = filesize,
    };
    
    strncpy(readRequest.name, sourceFileName, 8);
    strncpy(readRequest.ext, ext, 3);

    tssize_t readResult = fs_read(readRequest);

    if (readResult != 0)
    {
        return 3;
    }

    struct FAT32DriverRequest writeRequest = {
        .buf = readRequest.buf,
        .name = {0},
        .ext = {0},
        .parent_cluster_number = readRequest.parent_cluster_number,
        .buffer_size = readRequest.buffer_size,
    };

    strncpy(writeRequest.name, destinationFileName, 8);
    strncpy(writeRequest.ext, readRequest.ext, 3);

    tssize_t writeResult = fs_write(writeRequest);

    if (writeResult != 0)
    {
        return 4;
    }
    return 0;
}

int rm(struct CWDdata cwd_data, char *fileName)
{
    struct FAT32DirectoryTable dummy_read_dir_request;
    memset(&dummy_read_dir_request, 0, sizeof(dummy_read_dir_request));
    struct FAT32DriverRequest dirRequest = {
        .buf = &dummy_read_dir_request,
        .name = {0},
        .ext = {0},
        .parent_cluster_number = cwd_data.currentCluster,
        .buffer_size = CLUSTER_SIZE,
    };

    strncpy(dirRequest.name, cwd_data.cwdName, 8);
    strncpy(dirRequest.ext, "\0\0\0", 3);

    tssize_t dirReadResult = fs_read_dir(dirRequest);

    if (dirReadResult != 0)
    {
        return 1;
    }

    char ext[3];
    uint32_t filesize;
    struct FAT32DirectoryTable *dirtable = (struct FAT32DirectoryTable *)dirRequest.buf;
    int getData = -1;
    for (tssize_t i = 0; i < CLUSTER_SIZE / sizeof(struct FAT32DirectoryEntry); i++)
    {
        struct FAT32DirectoryEntry entry = dirtable->table[i];
        if (entry.name[0] == '\0')
        {
            break;
        }
        if (entry.attribute != ATTR_SUBDIRECTORY &&
            strcmp(entry.name, fileName) == 0)
        {
            strncpy(ext, entry.ext, 3);
            filesize = entry.filesize;
            getData = 1;
            break;
        }
    }

    if (getData == -1)
    {
        return 2;
    }

    struct ClusterBuffer dummy_read_request[filesize/sizeof(struct ClusterBuffer)];
    struct FAT32DriverRequest readRequest = {
        .buf = &dummy_read_request,
        .name = {0},
        .ext = {0},
        .parent_cluster_number = cwd_data.currentCluster,
        .buffer_size = filesize,
    };
    
    strncpy(readRequest.name, fileName, 8);
    strncpy(readRequest.ext, ext, 3);

    tssize_t readResult = fs_read(readRequest);

    if (readResult != 0)
    {
        return 3;
    }

    struct FAT32DriverRequest deleteRequest = {
        .buf = readRequest.buf,
        .name = {0},
        .ext = {0},
        .parent_cluster_number = readRequest.parent_cluster_number,
        .buffer_size = readRequest.buffer_size,
    };

    strncpy(deleteRequest.name, fileName, 8);
    strncpy(deleteRequest.ext, readRequest.ext, 3);

    tssize_t deleteResult = fs_delete(deleteRequest);

    if (deleteResult != 0)
    {
        return 4;
    }
    return 0;
}

/**
 * @param cwd_data current working directory
 * @param filename file name
*/
void whereis(struct CWDdata cwd_data, char *filename){
    // print filename
    log(filename);
    log(": ");

    char name[8];
    if (strlen(filename) <= 8)
    {
        int i = 0;
        while (i < 8)
        {
            if (i < strlen(filename))
            {
                name[i] = filename[i];
        }
            else
            {
                name[i] = '\0';
    }
            i++;
        }
    }
    else
    {
        return;
    }
    //read root directory
    struct FAT32DirectoryTable dir_table = {0};
    struct FAT32DriverRequest req = {
            .buf                   = &dir_table,
            .name                  = {0},
            .ext                   = {0},
            .parent_cluster_number = cwd_data.currentCluster,
            .buffer_size           = CLUSTER_SIZE,
    };
    strncpy(req.name, name, 8);
    strncpy(req.ext, "\0\0\0", 3);
    // int8_t retcode = fs_read_dir(req);

    if(retcode==0){
        // search file
        bool found = FALSE;
        uint32_t index = 1;
        char path[100]="/root\0\0\0\0";

        struct FAT32DirectoryTable *directory_table = (struct FAT32DirectoryTable *)req.buf;
        while (!found && index < 8*CLUSTER_SIZE/sizeof(struct FAT32DirectoryEntry)) {
            struct FAT32DirectoryEntry entry = directory_table->table[index];
            if(entry.attribute==ATTR_SUBDIRECTORY){
                if (strcmp(entry.name, req.name) == 0 && entry.undelete) {
                    // search that file here 

                    //print path
                    strcat(path, "/");
                    strcat(path, req.name);
                    //print filename
                    log(path);

                    //print extension if exist
                    if (entry.ext[0] != '\0')
                    {
                        log(".");
                        log(entry.ext);
                    }
                    log("     ");
                    found= TRUE;
                }else{
                    int i = index;
                    // search that folder here 
                    //looping search
                    while(directory_table->table[i].name[0]!='\0'){
                        struct FAT32DirectoryEntry entry_temp = directory_table->table[i];
                        struct FAT32DriverRequest req_temp={
                                .buf                   = &dir_table,
                                .name                  = {0},
                                .ext                   = {0},
                                .parent_cluster_number = (((uint32_t) entry.cluster_high << 16) | entry.cluster_low),
                                .buffer_size           = CLUSTER_SIZE,
                        };
                        strncpy(req_temp.name, entry_temp.name, 8);
                        strncpy(req_temp.ext, entry_temp.ext, 3);
                        int retcode_temp = fs_read_dir(req_temp);
                    
                        if(retcode_temp==0){
                            bool found_temp = FALSE;
                            int index_temp = 0;
                            while(!found_temp){
                                struct FAT32DirectoryEntry entry_temp2 = directory_table->table[index_temp];
                                if (strcmp(entry_temp2.name, req.name) == 0 && entry_temp2.undelete) {
                                    // search that file here 

                                    //print path
                                    strcat(path, "/");
                                    strcat(path, req_temp.name);
                                    strcat(path, "/");
                                    strcat(path, req.name);
                                    //print filename
                                    log(path);

                                    //print extension if exist
                                    if (entry_temp2.ext[0] != '\0')
                                    {
                                        log(".");
                                        log(entry_temp2.ext);
                                    }
                                    found_temp= TRUE;
                                    found = TRUE;
                                    log("     ");
                                }
                                index_temp++;
                            }
                        }
                        i++;
                    }
                }
                                    
            }else{
                if (strcmp(entry.name, req.name) == 0 && entry.undelete) {
                    // search that file here 

                    //print path
                    strcat(path, "/");
                    strcat(path, req.name);
                    //print filename
                    log(path);

                    //print extension if exist
                    if (entry.ext[0] != '\0')
                    {
                        log(".");
                        log(entry.ext);
                    }
                    found= TRUE;
                    log("     ");
                }
            }
            index++;
        }
    }
    log("\n");
}
