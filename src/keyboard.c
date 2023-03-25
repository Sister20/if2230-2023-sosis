#include "lib-header/keyboard.h"
#include "lib-header/portio.h"
#include "lib-header/framebuffer.h"
#include "lib-header/stdmem.h"

static struct KeyboardDriverState keyboard_state = {
    .read_extended_mode = FALSE,
    .keyboard_input_on = FALSE,
    .buffer_index = 0,
    .keyboard_buffer = {'\0'}};

static int keyboard_cursor_row = 0;
static int keyboard_cursor_col = 0;

const char keyboard_scancode_1_to_ascii_map[256] = {
    0,
    0x1B,
    '1',
    '2',
    '3',
    '4',
    '5',
    '6',
    '7',
    '8',
    '9',
    '0',
    '-',
    '=',
    '\b',
    '\t',
    'q',
    'w',
    'e',
    'r',
    't',
    'y',
    'u',
    'i',
    'o',
    'p',
    '[',
    ']',
    '\n',
    0,
    'a',
    's',
    'd',
    'f',
    'g',
    'h',
    'j',
    'k',
    'l',
    ';',
    '\'',
    '`',
    0,
    '\\',
    'z',
    'x',
    'c',
    'v',
    'b',
    'n',
    'm',
    ',',
    '.',
    '/',
    0,
    '*',
    0,
    ' ',
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    '-',
    0,
    0,
    0,
    '+',
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,

    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
};

void keyboard_state_activate(void)
{
  keyboard_state.keyboard_input_on = TRUE;
}

void keyboard_state_deactivate(void)
{
  keyboard_state.keyboard_input_on = FALSE;
}

void get_keyboard_buffer(char *buf)
{
  for (int i = 0; i < KEYBOARD_BUFFER_SIZE; i++)
  {
    buf[i] = keyboard_state.keyboard_buffer[i];
  }
}

bool is_keyboard_blocking(void)
{
  return keyboard_state.keyboard_input_on;
}

void keyboard_isr(void)
{
  if (!keyboard_state.keyboard_input_on)
    keyboard_state.buffer_index = 0;
  else
  {
    uint8_t scancode = in(KEYBOARD_DATA_PORT);
    char mapped_char = keyboard_scancode_1_to_ascii_map[scancode];
    framebuffer_set_cursor(keyboard_cursor_row, keyboard_cursor_col);
    switch (mapped_char)
    {
    case 0:
      framebuffer_write(keyboard_cursor_row, keyboard_cursor_col, ' ', 0xF, 0);
      break;
    case '\b':
      keyboard_state.buffer_index--;
      framebuffer_write(keyboard_cursor_row, keyboard_cursor_col, ' ', 0xF, 0);
      if (keyboard_cursor_row != 0 || keyboard_cursor_col != 0)
      {
        if (keyboard_cursor_col == 0)
        {
          keyboard_cursor_row = keyboard_cursor_row - 1;
          keyboard_cursor_col = 79;
        }
        else
          keyboard_cursor_col = keyboard_cursor_col - 1;
      }
      break;

    case '\n':
      keyboard_cursor_row++;
      keyboard_cursor_col = 0;
      keyboard_state.keyboard_buffer[keyboard_state.buffer_index] = '\0';
      keyboard_state.buffer_index = 0;
      keyboard_state_deactivate();
      break;

    default:
      keyboard_state.keyboard_buffer[keyboard_state.buffer_index] = mapped_char;
      keyboard_state.buffer_index++;
      framebuffer_write(keyboard_cursor_row, keyboard_cursor_col, mapped_char, 0xF, 0);
      if (keyboard_cursor_col == 80)
      {
        keyboard_cursor_row = keyboard_cursor_row + 1;
        keyboard_cursor_col = 0;
      }
      else
        keyboard_cursor_col = keyboard_cursor_col + 1;
    }

    framebuffer_set_cursor(keyboard_cursor_row, keyboard_cursor_col);
  }
  pic_ack(IRQ_KEYBOARD);
}
