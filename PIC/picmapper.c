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

    outb(MASTER_DATA_PORT, 0x20); //master starts interrupts at interrupt 32
    outb(SLAVE_DATA_PORT, 0x28); //slave starts interrupts at interrupt 40

    outb(MASTER_DATA_PORT, 0x04); // slave on irq2
    outb(SLAVE_DATA_PORT, 0x02); //slave cascade identity is 2

    outb(MASTER_DATA_PORT, 0x01); // pic must use 8086 mode
    outb(SLAVE_DATA_PORT, 0x01);  // pic must use 8086 mode


    outb(MASTER_DATA_PORT, master_mask);
    outb(SLAVE_DATA_PORT, slave_mask);
}

void ack_interrupt()
{
    outb(0x20, 0x20);
}