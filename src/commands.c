#include "lib-header/commands.h"

int cd(struct CWDdata *cwd_data, char *folderName)
{
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
        return -1;
    }

    if (strcmp(folderName, "..\0") == 0)
    {
        if (cwd_data->currentCluster == ROOT_CLUSTER_NUMBER)
        {
            return 0;
        }
        else
        {
            // move back to its parent
            struct FAT32DriverRequest request = {
                .buf = (uint8_t *)0,
                .name = {*cwd_data->cwdName}, // change this to parent name
                .ext = "\0\0\0",
                .parent_cluster_number = cwd_data->currentCluster,
                .buffer_size = 0,
            };
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
                        cwd_data->currentCluster = (((uint32_t)entry.cluster_high << 16) | entry.cluster_low);
                        strcpy(cwd_data->cwdName, entry.name);
                        break;
                    }
                    index++;
                }
            }
            return retcode;
        }
    }
    else
    {
        struct FAT32DriverRequest request = {
            .buf = (uint8_t *)0,
            .ext = {0},
            .name = {0},
            .parent_cluster_number = cwd_data->currentCluster,
            .buffer_size = CLUSTER_SIZE,
        };
        strcpy(request.name, name);
        strcpy(request.ext, "\0\0\0");
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
                    cwd_data->currentCluster = (((uint32_t)entry.cluster_high << 16) | entry.cluster_low);
                    strcpy(cwd_data->cwdName, entry.name);
                    break;
                }
                index++;
            }
        }
        return retcode;
    }
}

void ls(uint32_t clusterNumber)
{
    if (clusterNumber) {

    }
    return;
}

void mkdir(uint32_t clusterNumber, char *dirName)
{
    struct FAT32DriverRequest request = {
        .buf = 0,
        .name = {* dirName},
        .ext = "\0\0\0",
        .parent_cluster_number = clusterNumber,
        .buffer_size = 0,
    };
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

void cat(uint32_t clusterNumber, char *fileName, char *fileExt)
{
    struct ClusterBuffer cbuf[4];
    struct FAT32DriverRequest request = {
        .buf = cbuf,
        .name = {* fileName},
        .ext = {* fileExt},
        .parent_cluster_number = clusterNumber,
        .buffer_size = 4 * CLUSTER_SIZE,
    };
    int32_t retcode;
    syscall(FS_READ, (uint32_t)&request, (uint32_t)&retcode, 0);

    switch (retcode)
    {
    case 0:
    {
        syscall(TEXT_OUTPUT, (uint32_t) "cat: File read\n", 15, 0xF);
        // chore: output file contents
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

int cp(uint32_t clusterNumber, char* sourceFileName, char* destinationFileName){
    struct FAT32DriverRequest dirRequest = {
        .buf                   = (uint8_t*) 0,
        .name                  = "",
        .ext                   = "",
        .parent_cluster_number = clusterNumber,
        .buffer_size           = 0,
    };

    tssize_t dirReadResult = fs_read_dir(dirRequest);

    if(dirReadResult != 0){
        return 1;
    }

    struct FAT32DirectoryEntry sourceFileEntry = {0};
    struct FAT32DirectoryTable* dirtable = (struct FAT32DirectoryTable *) dirRequest.buf;
    int getData = -1;
    for(tssize_t i = 0; i< CLUSTER_SIZE/sizeof(struct FAT32DirectoryEntry); i++){
        struct FAT32DirectoryEntry entry = dirtable->table[i];
        if(entry.name == '\0'){
            break;
        }
        if(entry.attribute != ATTR_SUBDIRECTORY &&
            memcmp(dirtable->table->name, sourceFileName, 8) == 0){
            sourceFileEntry = entry;
            getData = 1;
            break;
        }
    }

    if(getData == -1){
        return 2;
    }

    struct FAT32DriverRequest readRequest = {
        .buf                   = (uint8_t*) 0,
        .name                  = sourceFileEntry.name,
        .ext                   = sourceFileEntry.ext,
        .parent_cluster_number = clusterNumber,
        .buffer_size           = sourceFileEntry.filesize,
    };

    tssize_t readResult = fs_read(readRequest);
    if(readResult != 0){
        return 3;
    }

    struct FAT32DriverRequest writeRequest = {
        .buf                   = readRequest.buf,
        .name                  = destinationFileName,
        .ext                   = readRequest.ext,
        .parent_cluster_number = clusterNumber,
        .buffer_size           = readRequest.buffer_size,
    };

    tssize_t writeResult = fs_write(writeRequest);

    if(writeResult != 0){
        return 4;
    }
    return 0;
}

int rm(uint32_t clusterNumber, char* fileName){
    struct FAT32DriverRequest dirRequest = {
        .buf                   = (uint8_t*) 0,
        .name                  = "",
        .ext                   = "",
        .parent_cluster_number = clusterNumber,
        .buffer_size           = 0,
    };

    tssize_t dirReadResult = fs_read_dir(dirRequest);

    if(dirReadResult != 0){
        return 1;
    }

    struct FAT32DirectoryEntry sourceFileEntry = {0};
    struct FAT32DirectoryTable* dirtable = (struct FAT32DirectoryTable *) dirRequest.buf;
    int getData = -1;
    for(tssize_t i = 0; i< CLUSTER_SIZE/sizeof(struct FAT32DirectoryEntry); i++){
        struct FAT32DirectoryEntry entry = dirtable->table[i];
        if(entry.name == '\0'){
            break;
        }
        if(entry.attribute != ATTR_SUBDIRECTORY &&
            memcmp(dirtable->table->name, fileName, 8) == 0){
            sourceFileEntry = entry;
            getData = 1;
            break;
        }
    }

    if(getData == -1){
        return 2;
    }

    struct FAT32DriverRequest readRequest = {
        .buf                   = (uint8_t*) 0,
        .name                  = sourceFileEntry.name,
        .ext                   = sourceFileEntry.ext,
        .parent_cluster_number = clusterNumber,
        .buffer_size           = sourceFileEntry.filesize,
    };

    tssize_t readResult = fs_read(readRequest);
    if(readResult != 0){
        return 3;
    }

    struct FAT32DriverRequest deleteRequest = {
        .buf                   = readRequest.buf,
        .name                  = fileName,
        .ext                   = readRequest.ext,
        .parent_cluster_number = clusterNumber,
        .buffer_size           = readRequest.buffer_size,
    };

    tssize_t deleteResult = fs_delete(deleteRequest);

    if(deleteResult != 0){
        return 4;
    }
    return 0;
}