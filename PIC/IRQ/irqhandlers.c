#include <stdint.h>
#include <stddef.h>
#include "../picmapper.h"
#include "../../ISR/idthandler.h"
#include "../../strlib.h"
#include "irqhandlers.h"

extern void irq32();
extern void irq33();
extern void irq34();
extern void irq35();
extern void irq36();
extern void irq37();
extern void irq38();
extern void irq39();
extern void irq40();
extern void irq41();
extern void irq42();
extern void irq43();
extern void irq44();
extern void irq45();
extern void irq46();
extern void irq47();

void process_keyboard_interrupt();

void (*irq_handlers[16])() =
{
    irq32,
    irq33,
    irq34,
    irq35,
    irq36,
    irq37,
    irq38,
    irq39,
    irq40,
    irq41,
    irq42,
    irq43,
    irq44,
    irq45,
    irq46,
    irq47,
};


void irq_handlers_init()
{
    uint8_t mask = inb(0x21);
    mask |= 0x01;        // set bit 0 (mask IRQ0)
    outb(0x21, mask);
    remap_pic_interrupts();
    for(int i = 32; i < 48; i++)
    {
        idt_set_gate(i, (uint32_t)irq_handlers[i-32]);
    }
}

void irq_handler(struct irq_frame* register_state)
{
    if(register_state->irq == 33)
    {
        process_keyboard_interrupt();
    }

    if (register_state->irq >= 40) //check if the interrupt came from the slave
        outb(0xA0, 0x20); // slave PIC

    outb(0x20, 0x20);     // master PIC
}

void put_mod_result(uint32_t x, uint32_t y)
{
    put_char('\n');
    char buffer[32];
    put_str(uint32_to_str(x % y, buffer));
    put_char('\n');
}