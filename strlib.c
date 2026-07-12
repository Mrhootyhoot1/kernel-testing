#include <stdint.h>
#define VGA_WIDTH 80
#define VGA_HEIGHT 25

uint16_t currentChar = 0;

//returns the size of the new string
char* uint32_to_str(uint32_t num, char* buffer)
{
    if (num == 0)
    {
        buffer[0] = '0';
        buffer[1] = '\0';
        return buffer;
    }

    int i = 0;

    while (num > 0)
    {
        buffer[i++] = (num % 10) + '0';
        num /= 10;
    }

    for (int j = 0; j < i / 2; j++)
    {
        char tmp = buffer[j];
        buffer[j] = buffer[i - j - 1];
        buffer[i - j - 1] = tmp;
    }

    buffer[i] = '\0';
    return buffer;
}

uint32_t str_to_uint32(char* str, int length)
{
    uint32_t number = 0;
    if(str[length] == '\0')
    {
        length -= 1;
    }

    int multiplier = 1;
    for (int i = length; i >= 0; i--)
    {
        uint32_t current = str[i] - '0';
        current = current * multiplier;
        number += current;
        multiplier *= 10;
    }
    return number;
}

void clear()
{
    currentChar = 0;
}

void put_char(char c)
{
    if(currentChar+1 > VGA_HEIGHT * VGA_WIDTH)
    {
        clear();
    }

    volatile uint16_t* vga = (uint16_t*)0xB8000;
    if(c == '\n')
    {
        currentChar += VGA_WIDTH - (currentChar % VGA_WIDTH);
        return;
    }
    else if(c == '\b' && currentChar > 0)
    {
        currentChar-=1;
        vga[currentChar] = ((uint16_t)0x07 << 8) | (uint8_t)' ';
        return;
    }

    vga[currentChar] = ((uint16_t)0x07 << 8) | (uint8_t)c;
    currentChar++;
}

void put_str(char* str)
{
    for(int i = 0; str[i] != '\0'; i++)
    {
        put_char(str[i]);
    }
}

uint32_t strlen(const char* str)
{
    uint32_t length = 0;

    for (int i = 0; str[i] != '\0'; i++)
    {
        length++;
    }
    return length;
}

char* strcat(char* source, char* dest)
{
    int destLength = strlen(dest);
    int i;
    for (i = 0; source[i] != '\0'; i++)
    {
        dest[destLength + i] = source[i];
    }
    dest[i + destLength] = '\0';
    return dest;
}

char* strcpy(char* dest, const char* source)
{
    int i = 0;

    while (source[i] != '\0')
    {
        dest[i] = source[i];
        i++;
    }

    dest[i] = '\0';

    return dest;
}