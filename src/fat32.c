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
