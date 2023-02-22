#include "lib-header/portio.h"
#include "lib-header/stdtype.h"
#include "lib-header/stdmem.h"
#include "lib-header/gdt.h"
#include "lib-header/framebuffer.h"
#include "lib-header/kernel_loader.h"

void kernel_setup(void) {
    enter_protected_mode(&_gdt_gdtr);
    framebuffer_clear();
    framebuffer_write(3, 8,  'H', 2, 0xF);
    framebuffer_write(3, 9,  'a', 2, 0xF);
    framebuffer_write(3, 10, 'i', 2, 0xF);
    framebuffer_write(3, 11, '!', 2, 0xF);
    framebuffer_set_cursor(3, 10);
    while (TRUE);
}
