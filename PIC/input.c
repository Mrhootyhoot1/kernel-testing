#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>
#include "picmapper.h"
#include "../ISR/idthandler.h"
#include "../strlib.h"
#include "IRQ/irqhandlers.h"
#include "input.h"

uint8_t input_buffer[1024];
uint32_t keyboard_write_index;
uint32_t keyboard_read_index;

char scancode_to_ascii[128] =
{
    0, 27, '1', '2', '3', '4', '5', '6',
    '7', '8', '9', '0', '-', '=', '\b', '\t',
    'q', 'w', 'e', 'r', 't', 'y', 'u', 'i',
    'o', 'p', '[', ']', '\n', 0, 'a', 's',
    'd', 'f', 'g', 'h', 'j', 'k', 'l', ';',
    '\'', '`', 0, '\\', 'z', 'x', 'c', 'v',
    'b', 'n', 'm', ',', '.', '/', 0, '*',
    0, ' ', /* ... */
};

bool scancode_pressed[128] = { false };



void process_keyboard_interrupt()
{
    uint8_t scancode = inb(0x60);
    if(scancode & 0x80)
    {
        scancode &= 0x7F;
        scancode_pressed[scancode] = false;
        return;
    }

    input_buffer[keyboard_write_index] = scancode;
    scancode_pressed[scancode] = true;
    keyboard_write_index++;

    put_char(scancode_to_ascii[scancode]);
    
    if(keyboard_write_index >= 1024)
    {
        keyboard_write_index = 0;
    }
}

bool key_pressed(enum key_code code)
{
    return scancode_pressed[code];
}

char get_char_from_scancode(uint8_t scancode)
{
    return scancode_to_ascii[scancode];
}

char read_char()
{
    if(keyboard_write_index == keyboard_read_index)
    {
        return '\0';
    }

    char c = input_buffer[keyboard_read_index];
    keyboard_read_index++;
    if(keyboard_read_index > (sizeof(input_buffer) - 1))
    {
        keyboard_read_index = 0;
    }
    return c;
}