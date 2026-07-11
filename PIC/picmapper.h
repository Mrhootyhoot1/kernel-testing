#include <stdint.h>

void remap_pic_interrupts();
uint8_t inb(uint16_t port);
void outb(uint16_t port, uint8_t value);