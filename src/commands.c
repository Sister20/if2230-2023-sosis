#include "lib-header/commands.h"

/**
 * @param clusterNumber current cluster number
 * @param path path to dir
*/
int cd(uint32_t* clusterNumber, char* folderName) {
    char name[8];
    if (strlen(folderName) <= 8) {
        int i = 0;
        while (i < 8) {
            if (i < strlen(folderName)) {
                name[i] = folderName[i];
            } else {
                name[i] = '\0';
            }
            i++;
        }
    }

    if (strcmp(folderName, "..\0") == 0) {
        return 0;
    } else {
        struct FAT32DriverRequest request = {
            .buf                   = (uint8_t*) 0,
            .name                  = {*name},
            .ext                   = "\0\0\0",
            .parent_cluster_number = *clusterNumber,
            .buffer_size           = 0,
        };
        int8_t retcode = fs_read_dir(request);
        if (retcode == 0) {
            bool found = FALSE;
            int index = 0;
            struct FAT32DirectoryTable *directory_table = (struct FAT32DirectoryTable *)request.buf;
            while (!found) {
                struct FAT32DirectoryEntry entry = directory_table->table[index];
                if (strcmp(entry.name, request.name) == 0 && entry.attribute == ATTR_SUBDIRECTORY) {
                    // search that folder here then get the cluster number
                    *clusterNumber = (((uint32_t) entry.cluster_high << 16) | entry.cluster_low);
                    break;
                }
                index++;
            }
        }
        return retcode;
    }


}

// void ls(uint32_t clusterNumber) {
    
// }
