#include <stdint.h>
#include <stddef.h>
#include "../strlib.h"
#include "../PIC/input.h"
#include "../ISR/idthandler.h"
#include "../Threading/Scheduler/scheduler.h"

extern void int0x80();

void undefined_interrupt_handler();

void (*software_interrupts[208])() =
{
    [0x80] = int0x80,
};

void init_software_interrupts()
{
    for(uint8_t i = 48; i < 255; i++)
    {
        if(software_interrupts[i] == NULL)
        {
            idt_set_gate(i, (uint32_t)undefined_interrupt_handler);
        }
        idt_set_gate(i, (uint32_t)software_interrupts[i]);
    }
}

struct CPUContext* syscall_handler(struct CPUContext* current_context);

struct CPUContext* software_interrupt_handler(struct CPUContext* current_context, uint32_t interrupt_number)
{
    switch (interrupt_number)
    {
        case 80:
            return syscall_handler(current_context);
            break;
    }
    return current_context;
}


void undefined_interrupt_handler()
{
    
}