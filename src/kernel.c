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

void kernel_setup(void)
{
    // enter_protected_mode(&_gdt_gdtr);
    // pic_remap();
    // initialize_idt();
    // framebuffer_clear();
    // framebuffer_set_cursor(0, 0);
    // activate_keyboard_interrupt();
    // while (TRUE)
    //     keyboard_state_activate();
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
        .ext                   = "uwu",
        .parent_cluster_number = ROOT_CLUSTER_NUMBER,
        .buffer_size           = 0,
    } ;

    write(request);  // Create folder "ikanaide"
    memcpy(request.name, "kano1\0\0\0", 8);
    write(request);  // Create folder "kano1"
    memcpy(request.name, "ikanaide", 8);
    int8_t ret = delete(request); // Delete first folder, thus creating hole in FS

    if (ret) {}
    memcpy(request.name, "daijoubu", 8);
    request.buffer_size = 5*CLUSTER_SIZE;
    write(request);  // Create fragmented file "daijoubu"

    // struct ClusterBuffer cbuf[5];
    for (uint32_t i = 0; i < 5; i++)
        for (uint32_t j = 0; j < CLUSTER_SIZE; j++)
            cbuf[i].buf[j] = i + 'b';

    struct ClusterBuffer readcbuf;
    read_clusters(&readcbuf, ROOT_CLUSTER_NUMBER+1, 1); 
    // If read properly, readcbuf should filled with 'a'

    memcpy(request.name, "newfile\0", 8);
    request.buf = cbuf;
    write(request);  // Create fragmented file "newfile"

    memcpy(request.name, "daijoubu", 8);
    request.buffer_size = CLUSTER_SIZE;
    int8_t read_ret = read(request);   // Failed read due not enough buffer size
    
    request.buffer_size = 5*CLUSTER_SIZE;
    read_ret = read(request);   // Success read on file "daijoubu"
    
    void *buf = request.buf;

    if (read_ret && buf) {};

    struct ClusterBuffer dbuf;
    for (uint32_t j = 0; j < CLUSTER_SIZE; j++)
        dbuf.buf[j] = 'c';

    struct FAT32DriverRequest request2 = {
        .buf                   = dbuf.buf,
        .name                  = "request2",
        .ext                   = "exe",
        .parent_cluster_number = ROOT_CLUSTER_NUMBER + 2,
        .buffer_size           = 0,
    } ;

    request2.buffer_size = CLUSTER_SIZE;
    request2.buf = dbuf.buf;
    memcpy(request2.name, "filedua\0", 8);
    int8_t res = write(request2);
    if (res) {}
    // enter_protected_mode(&_gdt_gdtr);
    // pic_remap();
    // initialize_idt();
    // activate_keyboard_interrupt();
    // framebuffer_clear();
    // framebuffer_set_cursor(0, 0);
    // initialize_filesystem_fat32();
    // keyboard_state_activate();

    // struct ClusterBuffer cbuf[5];
    // for (uint32_t i = 0; i < 5; i++)
    //     for (uint32_t j = 0; j < CLUSTER_SIZE; j++)
    //         cbuf[i].buf[j] = i + 'a';

    // struct FAT32DriverRequest request = {
    //     .buf                   = cbuf,
    //     .name                  = "ikanaide",
    //     .ext                   = "uwu",
    //     .parent_cluster_number = ROOT_CLUSTER_NUMBER,
    //     .buffer_size           = 0,
    // } ;

    // write(request);  // Create folder "ikanaide"
    // memcpy(request.name, "kano1\0\0\0", 8);
    // write(request);  // Create folder "kano1"
    // memcpy(request.name, "ikanaide", 8);
    // memcpy(request.ext, "\0\0\0", 3);
    // delete(request); // Delete first folder, thus creating hole in FS

    // memcpy(request.name, "daijoubu", 8);
    // request.buffer_size = 5*CLUSTER_SIZE;
    // write(request);  // Create fragmented file "daijoubu"

    // struct ClusterBuffer readcbuf;
    // read_clusters(&readcbuf, ROOT_CLUSTER_NUMBER+1, 1); 
    // // If read properly, readcbuf should filled with 'a'

    // request.buffer_size = CLUSTER_SIZE;
    // read(request);   // Failed read due not enough buffer size
    // request.buffer_size = 5*CLUSTER_SIZE;
    // read(request);   // Success read on file "daijoubu"

    // while (TRUE);

    while (TRUE);
}
