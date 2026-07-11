#include <stddef.h>
#include <stdint.h>
#include "picmapper.h"

#define MASTER_COMMAND_PORT 0x20
#define MASTER_DATA_PORT 0x21
#define SLAVE_COMMAND_PORT 0xA0
#define SLAVE_DATA_PORT 0xA1
#define START_INIT_COMMAND 0x11

void outb(uint16_t port, uint8_t value)
{
    __asm__ volatile ("outb %0, %1" : : "a"(value), "Nd"(port));
}

uint8_t inb(uint16_t port)
{
    uint8_t value;
    __asm__ volatile ("inb %1, %0" : "=a"(value) : "Nd"(port));
    return value;
}

/*
    Master interrupts: 0x20-0x27
    Slave interrupts: 0x28-0x30
*/

void remap_pic_interrupts()
{
    uint8_t master_mask = inb(0x21);
    uint8_t slave_mask  = inb(0xA1);

    outb(MASTER_COMMAND_PORT, START_INIT_COMMAND);
    outb(SLAVE_COMMAND_PORT, START_INIT_COMMAND);

    outb(MASTER_DATA_PORT, 0x20);
    outb(SLAVE_DATA_PORT, 0x28);

    outb(MASTER_DATA_PORT, 0x04); 
    outb(SLAVE_DATA_PORT, 0x02); 

    outb(MASTER_DATA_PORT, 0x01);
    outb(SLAVE_DATA_PORT, 0x01);


    outb(MASTER_DATA_PORT, master_mask);
    outb(SLAVE_DATA_PORT, slave_mask);
}