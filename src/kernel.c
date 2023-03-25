#include "lib-header/portio.h"
#include "lib-header/stdtype.h"
#include "lib-header/stdmem.h"
#include "lib-header/gdt.h"
#include "lib-header/idt.h"
#include "lib-header/interrupt.h"
#include "lib-header/framebuffer.h"
#include "lib-header/kernel_loader.h"
#include "lib-header/keyboard.h"

void kernel_setup(void) {
    // enter_protected_mode(&_gdt_gdtr);
    // framebuffer_clear();
    // framebuffer_write(3, 20,  'H', 0, 0xF);
    // framebuffer_write(4, 21,  'E', 0, 0xF);
    // framebuffer_write(3, 22, 'R', 0, 0xF);
    // framebuffer_write(4, 23, 'E', 0, 0xF);
    // framebuffer_write(3, 24, '!', 0, 0xF);
    // framebuffer_write(4, 25, '!', 0, 0xF);
    // framebuffer_set_cursor(0, 0);
    // while (TRUE);
    enter_protected_mode(&_gdt_gdtr);
    pic_remap();
    initialize_idt();
    framebuffer_clear();
    framebuffer_set_cursor(0, 0);
    activate_keyboard_interrupt();
    while (TRUE)
        keyboard_state_activate();
    // enter_protected_mode(&_gdt_gdtr);
    // pic_remap();
    // initialize_idt();
    // framebuffer_clear();
    // framebuffer_set_cursor(0, 0);
    // __asm__("int $0x4");
    // while (TRUE);
}
