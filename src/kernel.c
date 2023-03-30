#include "lib-header/portio.h"
#include "lib-header/stdtype.h"
#include "lib-header/stdmem.h"
#include "lib-header/gdt.h"
#include "lib-header/idt.h"
#include "lib-header/interrupt.h"
#include "lib-header/framebuffer.h"
#include "lib-header/kernel_loader.h"
#include "lib-header/keyboard.h"
#include "lib-header/fat32.h"

void kernel_setup(void) {
    enter_protected_mode(&_gdt_gdtr);
    pic_remap();
    initialize_idt();
    activate_keyboard_interrupt();
    framebuffer_clear();
    framebuffer_set_cursor(0, 0);
    initialize_filesystem_fat32();
    keyboard_state_activate();

    struct ClusterBuffer cbuf[5];
    for (uint32_t i = 0; i < 5; i++)
        for (uint32_t j = 0; j < CLUSTER_SIZE; j++)
            cbuf[i].buf[j] = i + 'a';

    struct FAT32DriverRequest request = {
        .buf                   = cbuf,
        .name                  = "ikanaide",
        .ext                   = "\0\0\0",
        .parent_cluster_number = ROOT_CLUSTER_NUMBER,
        .buffer_size           = 0,
    } ;

    write(request);  // Create folder "ikanaide"
    memcpy(request.name, "kano1\0\0\0", 8);
    write(request);  // Create folder "kano1"
    memcpy(request.name, "ikanaide", 8);
    delete(request); // Delete first folder, thus creating hole in FS

    memcpy(request.name, "daijoubu", 8);
    memcpy(request.ext, "uwu", 3);
    request.buffer_size = 5*CLUSTER_SIZE;
    write(request);  // Create fragmented file "daijoubu"

    struct ClusterBuffer readcbuf;
    read_clusters(&readcbuf, ROOT_CLUSTER_NUMBER+1, 1); 
    // If read properly, readcbuf should filled with 'a'

    request.buffer_size = CLUSTER_SIZE;
    read(request);   // Failed read due not enough buffer size
    request.buffer_size = 5*CLUSTER_SIZE;
    read(request);   // Success read on file "daijoubu"

    memcpy(request.name, "daijoubu", 8);
    memcpy(request.ext, "uwu", 3);
    request.buffer_size = 5*CLUSTER_SIZE;
    delete(request);
    // Delete daijoubu, content is free to be overwrite

    struct ClusterBuffer dbuf[5];
    for (uint32_t i = 0; i < 5; i++)
        for (uint32_t j = 0; j < CLUSTER_SIZE; j++)
            dbuf[i].buf[j] = i + 'd';
    request.buf = dbuf;
    write(request);
    // Rewrite daijoubu, overwrite 'a's with 'd's

    // test: delete folder with contents
    struct ClusterBuffer ebuf;
    for (uint32_t j = 0; j < CLUSTER_SIZE; j++)
        ebuf.buf[j] = 'c';
    struct FAT32DriverRequest request2 = {
        .buf                   = ebuf.buf,
        .name                  = "request2",
        .ext                   = "\0\0\0",
        .parent_cluster_number = ROOT_CLUSTER_NUMBER + 2,
        .buffer_size           = 0,
    };

    memcpy(request2.name, "folder0\0", 8);
    write(request2);
    memcpy(request2.name, "folder1\0", 8);
    write(request2);
    memcpy(request2.name, "folder2\0", 8);
    write(request2);
    // write folders in kano1
    

    memcpy(request.name, "kano1\0\0\0", 8);
    memcpy(request.ext, "\0\0\0", 3);
    request.buffer_size = 0;
    delete(request);
    // Failt to delete, kano1 has contents
    delete(request2);
    // Deletes folder2 from kano1

    while (TRUE);
}
