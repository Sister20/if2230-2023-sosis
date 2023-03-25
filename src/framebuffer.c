#include "lib-header/framebuffer.h"
#include "lib-header/stdtype.h"
#include "lib-header/stdmem.h"
#include "lib-header/portio.h"

#define VGA_WIDTH               80
#define VGA_HEIGHT              25

static uint16_t* framebuffer = (uint16_t*) 0xB8000;
static uint8_t cursor_row = 0;
static uint8_t cursor_col = 0;
static uint8_t foreground_color = 0;
static uint8_t background_color = 0;

void framebuffer_set_cursor(uint8_t r, uint8_t c) {
    // Ensure row and column are within bounds
  if (r >= VGA_HEIGHT || c >= VGA_WIDTH) {
    return;
  }

  uint16_t pos = r * VGA_WIDTH + c;
 
	out(0x3D4, 0x0F);
	out(0x3D5, (uint8_t) (pos & 0xFF));
	out(0x3D4, 0x0E);
	out(0x3D5, (uint8_t) ((pos >> 8) & 0xFF));
}

void framebuffer_write(uint8_t row, uint8_t col, char c, uint8_t fg, uint8_t bg) {
    uint16_t attrib = (bg << 4) | (fg & 0x0F);
    volatile uint16_t * where;
    where = (volatile uint16_t *)0xB8000 + (row * 80 + col);
    *where = c | (attrib << 8);
}

void framebuffer_clear(void) {
    uint8_t blank = (background_color << 4) | (foreground_color & 0xF);
    uint16_t space = (blank << 8) | ' ';
    for (int i = 0; i < 80 * 25; i++) {
        framebuffer[i] = space;
    }
    cursor_row = 0;
    cursor_col = 0;
}

uint16_t framebuffer_get_cursor(void) {
    uint16_t pos = 0;
    out(CURSOR_PORT_CMD, 0x0F);
    pos |= in(CURSOR_PORT_DATA);
    out(CURSOR_PORT_CMD, 0x0E);
    pos |= ((uint16_t) in(CURSOR_PORT_DATA)) << 8;
    return pos;
}
