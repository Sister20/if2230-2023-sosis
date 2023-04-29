#include "lib-header/fat32-no-cmos.h"

static struct FAT32DriverState driver_state = {0};

const uint8_t fs_signature[BLOCK_SIZE] = {
    'C',
    'o',
    'u',
    'r',
    's',
    'e',
    ' ',
    ' ',
    ' ',
    ' ',
    ' ',
    ' ',
    ' ',
    ' ',
    ' ',
    ' ',
    'D',
    'e',
    's',
    'i',
    'g',
    'n',
    'e',
    'd',
    ' ',
    'b',
    'y',
    ' ',
    ' ',
    ' ',
    ' ',
    ' ',
    'L',
    'a',
    'b',
    ' ',
    'S',
    'i',
    's',
    't',
    'e',
    'r',
    ' ',
    'I',
    'T',
    'B',
    ' ',
    ' ',
    'M',
    'a',
    'd',
    'e',
    ' ',
    'w',
    'i',
    't',
    'h',
    ' ',
    '<',
    '3',
    ' ',
    ' ',
    ' ',
    ' ',
    '-',
    '-',
    '-',
    '-',
    '-',
    '-',
    '-',
    '-',
    '-',
    '-',
    '-',
    '2',
    '0',
    '2',
    '3',
    '\n',
    [BLOCK_SIZE - 2] = 'O',
    [BLOCK_SIZE - 1] = 'k',
};

void create_fat32(void)
{
    // Write the file system signature to the boot sector
    write_blocks(fs_signature, BOOT_SECTOR, 1);

    struct FAT32DirectoryTable directory_table = {0};
    // Create a buffer to hold the FAT data
    struct FAT32FileAllocationTable fat = {0};
    fat.cluster_map[0] = CLUSTER_0_VALUE;
    fat.cluster_map[1] = CLUSTER_1_VALUE;
    fat.cluster_map[2] = FAT32_FAT_END_OF_FILE;
    init_directory_table(&directory_table, "root\0\0\0\0", ROOT_CLUSTER_NUMBER);
    write_clusters(&directory_table, ROOT_CLUSTER_NUMBER, 1);

    // Write the FAT to the disk
    write_clusters(&fat, FAT_CLUSTER_NUMBER, 1);
}

void initialize_filesystem_fat32(void)
{
    // Check if the storage is empty
    if (is_empty_storage())
    {
        // Create a new FAT32 file system
        create_fat32();
    }
    else
    {
        // Read the FAT from the disk
        read_clusters(&driver_state.fat_table, FAT_CLUSTER_NUMBER, 1);
    }
}

bool is_empty_storage()
{
    uint8_t boot_sector[BLOCK_SIZE];

    // read boot sector from disk
    read_blocks(boot_sector, BOOT_SECTOR, 1);
    return memcmp(boot_sector, fs_signature, BLOCK_SIZE);
}

uint32_t cluster_to_lba(uint32_t cluster)
{
    return (cluster * CLUSTER_BLOCK_COUNT);
}

void init_directory_table(struct FAT32DirectoryTable *dir_table, char *name, uint32_t parent_dir_cluster)
{
    struct FAT32DirectoryEntry directory_entry = {0};
    memcpy(&directory_entry.name, name, 8);
    directory_entry.cluster_high = (uint16_t)(parent_dir_cluster >> 16);
    directory_entry.cluster_low = (uint16_t)(parent_dir_cluster & 0xFFFF);
    directory_entry.attribute = ATTR_SUBDIRECTORY;
    dir_table->table[0] = directory_entry;
}

void write_clusters(const void *ptr, uint32_t cluster_number, uint8_t cluster_count)
{
    uint32_t logical_block_address = cluster_number * CLUSTER_BLOCK_COUNT;
    uint8_t block_count = cluster_count * CLUSTER_BLOCK_COUNT;

    write_blocks(ptr, logical_block_address, block_count);
}

void read_clusters(void *ptr, uint32_t cluster_number, uint8_t cluster_count)
{
    uint32_t logical_block_address = cluster_number * CLUSTER_BLOCK_COUNT;
    uint8_t block_count = cluster_count * CLUSTER_BLOCK_COUNT;

    read_blocks(ptr, logical_block_address, block_count);
}

int8_t read_directory(struct FAT32DriverRequest request) {
    uint32_t tableSize = sizeof(struct FAT32DirectoryTable);
    if (request.buffer_size != tableSize) {
        return -1;
    }

    read_clusters(&driver_state.dir_table_buf, request.parent_cluster_number, 1);
    for (tssize_t i = 0; i < CLUSTER_SIZE / sizeof(struct FAT32DirectoryEntry); i++) {
        struct FAT32DirectoryEntry entry = driver_state.dir_table_buf.table[i];
        if (memcmp(entry.name, request.name, 8) == 0) {
            if (entry.attribute == ATTR_SUBDIRECTORY) {
                // Found folder
                read_clusters(request.buf, i, 1);
                return 0;
            }
        }
    }
    return 2;
}

int8_t read(struct FAT32DriverRequest request)
{
    // Read the directory table from the parent cluster number
    read_clusters(&driver_state.dir_table_buf, request.parent_cluster_number, 1);
    for (tssize_t i = 0; i < CLUSTER_SIZE / sizeof(struct FAT32DirectoryEntry); i++)
    {
        struct FAT32DirectoryEntry entry = driver_state.dir_table_buf.table[i];
        if (memcmp(entry.name, request.name, 8) == 0 && memcmp(entry.ext, request.ext, 3) == 0)
        {
            // Found the file
            if (entry.attribute == ATTR_SUBDIRECTORY)
            {
                // Not a file
                return 1;
            }
            if (request.buffer_size < entry.filesize)
            {
                // Not enough buffer
                return 2;
            }
            // Read the file data from the cluster number of the found file
            uint32_t cluster_number = ((uint32_t)entry.cluster_high << 16) | entry.cluster_low;
            int32_t bytes_left_to_read = entry.filesize;
            // uint16_t date;
            // get_date(&date);
            // driver_state.dir_table_buf.table[i].access_date = da`te;
            // write_clusters(&driver_state.dir_table_buf, cluster_number, 1);
            uint32_t bytes_to_read = (bytes_left_to_read < CLUSTER_SIZE) ? bytes_left_to_read : CLUSTER_SIZE;
            while (bytes_left_to_read > 0)
            {
                read_clusters(request.buf + entry.filesize - bytes_left_to_read, cluster_number, 1);
                bytes_left_to_read -= bytes_to_read;
                if (bytes_left_to_read < CLUSTER_SIZE)
                {
                    bytes_to_read = bytes_left_to_read;
                }
                else
                {
                    bytes_to_read = CLUSTER_SIZE;
                }
                cluster_number = driver_state.fat_table.cluster_map[cluster_number];
            }
            return 0; // Success
        }
    }
    return 3; // Not found
}

int8_t write(struct FAT32DriverRequest request)
{
    read_clusters(&driver_state.dir_table_buf, request.parent_cluster_number, 1);

    if (driver_state.dir_table_buf.table[0].attribute != ATTR_SUBDIRECTORY)
    {
        return 2;
    }

    bool foundName = FALSE;
    for (tssize_t i = 0; i < CLUSTER_SIZE / sizeof(struct FAT32DirectoryEntry); i++)
    {
        if (memcmp(&driver_state.dir_table_buf.table[i].name, &request.name, 8) == 0)
        {
            foundName = TRUE;
            if (foundName && memcmp(&request.ext, &driver_state.dir_table_buf.table[i].ext, 3) == 0)
            {
                return 1;
            }
            if (foundName && request.ext[0] == '\0' && driver_state.dir_table_buf.table[i].ext[0] == '\0')
            {
                return 1;
            }
        }
    }
    // no duplicate file / folder

    read_clusters(&driver_state.fat_table, FAT_CLUSTER_NUMBER, 1);

    driver_state.dir_table_buf.table[request.parent_cluster_number].user_attribute = UATTR_NOT_EMPTY;

    if (request.buffer_size != 0)
    {
        // save file data instead
        tssize_t prev_cluster_number = 0;
        tssize_t counter = 1;
        int32_t total_bytes = request.buffer_size;
        uint32_t fatIndex = 0;
        uint32_t file_size = 0;
        uint32_t written_data = 0;
        for (tssize_t i = 3; i < CLUSTER_MAP_SIZE; i++)
        {
            if (driver_state.fat_table.cluster_map[i] == 0 && total_bytes > 0)
            {
                if (counter >= 2)
                {
                    driver_state.fat_table.cluster_map[prev_cluster_number] = i;
                }
                else
                {
                    fatIndex = i;
                }
                write_clusters(request.buf + written_data, i, 1);
                written_data += total_bytes > CLUSTER_SIZE ? CLUSTER_SIZE : total_bytes;
                total_bytes -= CLUSTER_SIZE;
                if (total_bytes <= 0)
                {
                    file_size = written_data;
                    driver_state.fat_table.cluster_map[prev_cluster_number] = FAT32_FAT_END_OF_FILE;
                    break;
                }
                else
                {
                    prev_cluster_number = i;
                }
                counter++;
            }
        }

        struct FAT32DirectoryEntry new_entry = {0};

        for (tssize_t i = 0; i < CLUSTER_SIZE / sizeof(struct FAT32DirectoryEntry); i++)
        {
            if (driver_state.dir_table_buf.table[i].name[0] == '\0')
            {
                memcpy(&new_entry.name, request.name, 8);
                memcpy(&new_entry.ext, request.ext, 3);
                new_entry.user_attribute = UATTR_NOT_EMPTY;
                new_entry.cluster_high = fatIndex >> 16;
                new_entry.cluster_low = fatIndex & 0xFFFF;
                new_entry.undelete = TRUE;
                new_entry.filesize = file_size;
                // uint16_t date, time;
                // get_date(&date);

                // new_entry.create_date = date;
                // get_time(&time);

                // new_entry.create_time = time;
                driver_state.dir_table_buf.table[i] = new_entry;
                break;
            }
        }

        write_clusters(&driver_state.fat_table, FAT_CLUSTER_NUMBER, 1);
        write_clusters(&driver_state.dir_table_buf, request.parent_cluster_number, 1);

        return 0;
    }
    else
    {
        // create sub-directory
        uint32_t index = 0;
        for (tssize_t i = 0; i < CLUSTER_MAP_SIZE; i++)
        {
            if (driver_state.fat_table.cluster_map[i] == 0)
            {
                index = i;
                break;
            }
        }
        driver_state.fat_table.cluster_map[index] = FAT32_FAT_END_OF_FILE;

        for (tssize_t i = 0; i < CLUSTER_SIZE / sizeof(struct FAT32DirectoryEntry); i++)
        {
            struct FAT32DirectoryEntry new_entry = {0};
            bool track = driver_state.dir_table_buf.table[i].name[0] == '\0';
            if (track)
            {
                memcpy(&new_entry.name, request.name, 8);
                new_entry.attribute = ATTR_SUBDIRECTORY;
                new_entry.cluster_high = index >> 16;
                new_entry.cluster_low = index & 0xFFFF;
                new_entry.undelete = TRUE;
                new_entry.filesize = 0;
                // uint16_t date, time;
                // get_date(&date);

                // new_entry.create_date = date;
                // get_time(&time);

                // new_entry.create_time = time;
                driver_state.dir_table_buf.table[i] = new_entry;
                break;
            }
        }
        struct FAT32DirectoryTable sub_dir_table = {0};
        init_directory_table(&sub_dir_table, request.name, request.parent_cluster_number);

        write_clusters(&driver_state.fat_table, FAT_CLUSTER_NUMBER, 1);
        write_clusters(&driver_state.dir_table_buf, request.parent_cluster_number, 1);
        write_clusters(&sub_dir_table, index, 1);
        return 0;
    }
}

int8_t delete(struct FAT32DriverRequest request)
{
    // buffer to replace
    struct ClusterBuffer buf = {0};
    for (tssize_t i = 0; i < CLUSTER_SIZE; i++)
    {
        buf.buf[i] = 0;
    }
    // Read the directory table from the parent cluster number
    read_clusters(&driver_state.dir_table_buf, request.parent_cluster_number, 1);

    if (driver_state.dir_table_buf.table[0].attribute != ATTR_SUBDIRECTORY)
    {
        return -1;
    }

    for (tssize_t i = 1; i < CLUSTER_SIZE / sizeof(struct FAT32DirectoryEntry); i++)
    {
        struct FAT32DirectoryEntry entry = driver_state.dir_table_buf.table[i];
        if (memcmp(entry.name, request.name, 8) == 0 && (memcmp(entry.ext, request.ext, 3) == 0 || (entry.ext[0] == '\0' && request.ext[0] == '\0')) && entry.undelete)
        {
            // Found the file/folder
            if (entry.attribute == ATTR_SUBDIRECTORY)
            {
                // folder
                if (entry.user_attribute == UATTR_NOT_EMPTY)
                {
                    return 2; // not empty
                }
            }
            // Delete the directory entry
            driver_state.dir_table_buf.table[i].undelete = FALSE;
            uint32_t dir_cluster_number = (uint32_t)entry.cluster_high << 16 | entry.cluster_low;
            read_clusters(&driver_state.fat_table, FAT_CLUSTER_NUMBER, 1);

            // check linked clusters
            bool clean_delete = FALSE;
            // check linked clusters
            while (!clean_delete)
            {
                if (driver_state.fat_table.cluster_map[dir_cluster_number] == FAT32_FAT_END_OF_FILE)
                {
                    clean_delete = TRUE;
                }
                else
                {
                    uint32_t temp = dir_cluster_number;
                    write_clusters(&buf, temp, 1);
                    dir_cluster_number = driver_state.fat_table.cluster_map[dir_cluster_number];
                    driver_state.fat_table.cluster_map[temp] = 0;
                }
            }
            driver_state.fat_table.cluster_map[dir_cluster_number] = 0;
            memset(&driver_state.dir_table_buf.table[i], 0, sizeof(struct FAT32DirectoryEntry));
            // uint16_t date, time;
            // get_date(&date);
            // get_time(&time);

            // driver_state.dir_table_buf.table[0].modified_time = time;
            // driver_state.dir_table_buf.table[0].modified_date = date;
            write_clusters(&driver_state.fat_table, FAT_CLUSTER_NUMBER, 1);
            write_clusters(&driver_state.dir_table_buf, request.parent_cluster_number, 1);
            return 0; // Success
        }
    }
    return 1; // Not found
}