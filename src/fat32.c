#include "lib-header/fat32.h"

static struct FAT32DriverState driver_state;


const uint8_t fs_signature[BLOCK_SIZE] = {
    'C', 'o', 'u', 'r', 's', 'e', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ',  ' ',
    'D', 'e', 's', 'i', 'g', 'n', 'e', 'd', ' ', 'b', 'y', ' ', ' ', ' ', ' ',  ' ',
    'L', 'a', 'b', ' ', 'S', 'i', 's', 't', 'e', 'r', ' ', 'I', 'T', 'B', ' ',  ' ',
    'M', 'a', 'd', 'e', ' ', 'w', 'i', 't', 'h', ' ', '<', '3', ' ', ' ', ' ',  ' ',
    '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '2', '0', '2', '3', '\n',
    [BLOCK_SIZE-2] = 'O',
    [BLOCK_SIZE-1] = 'k',
};

/**
 * Create new FAT32 file system. Will write fs_signature into boot sector and 
 * proper FileAllocationTable (contain CLUSTER_0_VALUE, CLUSTER_1_VALUE, 
 * and initialized root directory) into cluster number 1
 */
void create_fat32(void) {
    // Write the file system signature to the boot sector
    write_blocks(fs_signature, BOOT_SECTOR, 1);

    struct FAT32DirectoryTable directory_table;
    // Create a buffer to hold the FAT data
    struct FAT32FileAllocationTable fat;
    fat.cluster_map[0] = CLUSTER_0_VALUE;
    fat.cluster_map[1] = CLUSTER_1_VALUE;
    fat.cluster_map[2] = FAT32_FAT_END_OF_FILE;
    init_directory_table(&directory_table, "root", ROOT_CLUSTER_NUMBER);
    write_clusters(&directory_table, ROOT_CLUSTER_NUMBER, 1);

    // Write the FAT to the disk
    write_clusters(fat.cluster_map, FAT_CLUSTER_NUMBER, 1);

}

void initialize_filesystem_fat32(void) {
    // Check if the storage is empty
    if (is_empty_storage()) {
        // Create a new FAT32 file system
        create_fat32();
    } else {
        // Read the FAT from the disk
        read_clusters(&driver_state.fat_table, FAT_CLUSTER_NUMBER, 1);
    }
}

bool is_empty_storage() {
    // allocate buffer to store boot sector
    uint8_t boot_sector[BLOCK_SIZE];

    // read boot sector from disk
    read_blocks(boot_sector, BOOT_SECTOR, 1);
    return memcmp(boot_sector, fs_signature, BLOCK_SIZE);
}

uint32_t cluster_to_lba(uint32_t cluster) {
    return (cluster * CLUSTER_BLOCK_COUNT) + 1;
}

void init_directory_table(struct FAT32DirectoryTable *dir_table, char *name, uint32_t parent_dir_cluster) {
    struct FAT32DirectoryEntry directory_entry;
    memcpy(&directory_entry.name, name, 8);
    directory_entry.cluster_high = (uint16_t)(parent_dir_cluster >> 16);
    directory_entry.cluster_low = (uint16_t)(parent_dir_cluster & 0xFFFF);
    directory_entry.attribute = ATTR_SUBDIRECTORY;
    dir_table->table[0] = directory_entry;
}

void write_clusters(const void *ptr, uint32_t cluster_number, uint8_t cluster_count) {
    // Calculate the logical block address from the cluster number
    uint32_t logical_block_address = cluster_number * CLUSTER_BLOCK_COUNT + 1;
    // Calculate the number of blocks to write from the cluster count
    uint8_t block_count = cluster_count * CLUSTER_BLOCK_COUNT + 1;
    // Call the write_blocks function with the calculated values
    write_blocks(ptr, logical_block_address, block_count);
}

void read_clusters(void *ptr, uint32_t cluster_number, uint8_t cluster_count) {
    // Calculate the logical block address from the cluster number
    uint32_t logical_block_address = cluster_number * CLUSTER_BLOCK_COUNT + 1;
    // Calculate the number of blocks to read from the cluster count
    uint8_t block_count = cluster_count * CLUSTER_BLOCK_COUNT + 1;
    // Call the read_blocks function with the calculated values
    read_blocks(ptr, logical_block_address, block_count);
}

/**
 *  FAT32 Folder / Directory read
 *
 * @param request buf point to struct FAT32DirectoryTable,
 *                name is directory name,
 *                ext is unused,
 *                parent_cluster_number is target directory table to read,
 *                buffer_size must be exactly sizeof(struct FAT32DirectoryTable)
 * @return Error code: 0 success - 1 not a folder - 2 not found - -1 unknown
 */
int8_t read_directory(struct FAT32DriverRequest request) {
    if (request.buffer_size != sizeof(struct FAT32DirectoryTable)) {
        return -1;
    }

    struct FAT32DirectoryTable *directory_table = (struct FAT32DirectoryTable *)request.buf;
    bool foundName = FALSE;
    for (int i = 0; i < CLUSTER_SIZE / sizeof(struct FAT32DirectoryEntry); i++) {
        if (memcmp(directory_table->table[i].name, request.name, 8) == 0) {
            foundName = TRUE;
            if (directory_table->table[i].attribute == ATTR_SUBDIRECTORY) {
                uint32_t cluster_number = ((uint32_t)directory_table->table[i].cluster_high << 16) | directory_table->table[i].cluster_low;
                read_clusters(directory_table, cluster_number, 1);
                return 0;
            }
        } 
    }
    if (foundName) {
        return 1;
    } else {
        return 2;
    }
}




/**
 * FAT32 read, read a file from file system.
 *
 * @param request All attribute will be used for read, buffer_size will limit reading count
 * @return Error code: 0 success - 1 not a file - 2 not enough buffer - 3 not found - -1 unknown
 */
int8_t read(struct FAT32DriverRequest request) {
    struct FAT32DirectoryTable dir_table;
    // Read the directory table from the parent cluster number
    read_clusters(&dir_table, request.parent_cluster_number, 1);
    for (int i = 0; i < CLUSTER_SIZE / sizeof(struct FAT32DirectoryEntry); i++) {


        struct FAT32DirectoryEntry entry = dir_table.table[i];
        if (memcmp(entry.name, request.name, 8) == 0 && memcmp(entry.ext, request.ext, 3) == 0) {
            // Found the file
            if (entry.attribute == ATTR_SUBDIRECTORY) {
                // Not a file
                return 1;
            }
            if (request.buffer_size < entry.filesize) {
                // Not enough buffer
                return 2;
            }
            // Read the file data from the cluster number of the found file
            uint32_t cluster_number = ((uint32_t)entry.cluster_high << 16) | entry.cluster_low;
            uint32_t cluster_count = (entry.filesize + CLUSTER_SIZE - 1) / CLUSTER_SIZE;
            read_clusters(request.buf, cluster_number, cluster_count);
            return 0; // Success
        }
    }
    return 3; // Not found
}

/**
 * FAT32 delete, delete a file or empty directory (only 1 DirectoryEntry) in file system.
 *
 * @param request buf and buffer_size is unused
 * @return Error code: 0 success - 1 not found - 2 folder is not empty - -1 unknown
 */
int8_t delete(struct FAT32DriverRequest request){
struct FAT32DirectoryTable dir_table;
    // Read the directory table from the parent cluster number
    read_clusters(&dir_table, request.parent_cluster_number, 1);
    for (int i = 0; i < CLUSTER_SIZE / sizeof(struct FAT32DirectoryEntry); i++) {
        struct FAT32DirectoryEntry entry = dir_table.table[i];
        if (memcmp(entry.name, request.name, 8) == 0 && memcmp(entry.ext, request.ext, 3) == 0) {
            // Found the file/folder
            if (entry.attribute == ATTR_SUBDIRECTORY) {
                //found folder
                struct FAT32DirectoryTable sub_dir_table;
                struct FAT32DriverRequest sub_request = {
                    .buf = &sub_dir_table,
                    .name = {0},
                    .ext = {0},
                    .parent_cluster_number = ((uint32_t)entry.cluster_high << 16) | entry.cluster_low,
                    .buffer_size = sizeof(sub_dir_table)
                };
                int8_t result = read_directory(sub_request);
                if (result != 0) {
                    return -1; //folder not found
                }
                // Check if the folder is empty
                bool is_empty = TRUE;
                for (int j = 0; j < CLUSTER_SIZE / sizeof(struct FAT32DirectoryEntry); j++) {
                    if (sub_dir_table.table[j].name[0] != 0 && sub_dir_table.table[j].name[0] != 0xE5) {
                        is_empty = FALSE;
                        break;
                    }
                }
                if (!is_empty) {
                    // Folder is not empty
                    return 2;
                }
            }
            // Delete the directory entry
            memset(&dir_table.table[i], 0, sizeof(struct FAT32DirectoryEntry));
            write_clusters(&dir_table, request.parent_cluster_number, 1);
            return 0; // Success
        }
    }
    return 1; // Not found
}