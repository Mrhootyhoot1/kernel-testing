#include <stdint.h>
#include <stddef.h>
char* uint32_to_str(uint32_t num, char* buffer);
char* uint32_to_hex(uint32_t num, char* buffer);
uint32_t str_to_uint32(char* str, int length);
void put_char(char c);
void put_str(char* str);
uint32_t strlen(const char* str);
char* strcat(char* dest, char* source);
char* strcpy(char* dest, const char* source);
void clear();
void* memmov(void* dest, const void *src, size_t size);
void* memcpy(void* dest, const void *src, size_t size);