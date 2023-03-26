#include "lib-header/stdtype.h"
#include "lib-header/fat32.h"
#include "lib-header/stdmem.h"


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

    // Create a buffer to hold the FAT data
    uint8_t fat[CLUSTER_SIZE] = {0};

    // Set the values for cluster 0 and cluster 1
    *(uint32_t*) &fat[0] = CLUSTER_0_VALUE;
    *(uint32_t*) &fat[4] = CLUSTER_1_VALUE;

    // Write the FAT to the disk
    write_clusters(fat, FAT_CLUSTER_NUMBER, 1);

    // Create a buffer to hold the root directory data
    uint8_t root_directory[CLUSTER_SIZE] = {0};

    // Write the root directory to the disk
    write_clusters(root_directory, ROOT_CLUSTER_NUMBER, 1);
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
    read_blocks(boot_sector, BOOT_SECTOR, CLUSTER_BLOCK_COUNT);

    return memcmp(boot_sector, fs_signature, BLOCK_SIZE) == 0;
}

uint32_t cluster_to_lba(uint32_t cluster) {

}

void init_directory_table(struct FAT32DirectoryTable *dir_table, char *name, uint32_t parent_dir_cluster) {

}

void write_clusters(const void *ptr, uint32_t cluster_number, uint8_t cluster_count) {
    // Calculate the logical block address from the cluster number
    uint32_t logical_block_address = cluster_number * CLUSTER_BLOCK_COUNT;
    // Calculate the number of blocks to write from the cluster count
    uint8_t block_count = cluster_count * CLUSTER_BLOCK_COUNT;
    // Call the write_blocks function with the calculated values
    write_blocks(ptr, logical_block_address, block_count);
}

void read_blocks(void *ptr, uint32_t logical_block_address, uint8_t block_count) {
    ATA_busy_wait();
    out(0x1F6, 0xE0 | ((logical_block_address >> 24) & 0xF));
    out(0x1F2, block_count);
    out(0x1F3, (uint8_t) logical_block_address);
    out(0x1F4, (uint8_t) (logical_block_address >> 8));
    out(0x1F5, (uint8_t) (logical_block_address >> 16));
    out(0x1F7, 0x20);

    uint16_t *target = (uint16_t*) ptr;

    for (uint32_t i = 0; i < block_count; i++) {
        ATA_busy_wait();
        ATA_DRQ_wait();
        for (uint32_t j = 0; j < HALF_BLOCK_SIZE; j++)
            target[j] = in16(0x1F0);
        // Note : uint16_t => 2 bytes, HALF_BLOCK_SIZE*2 = BLOCK_SIZE with pointer arithmetic
        target += HALF_BLOCK_SIZE;
    }
}
