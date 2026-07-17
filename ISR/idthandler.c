#include "idthandler.h"
#include "../strlib.h"
#include <stdint.h>
#include "../PIC/picmapper.h"

extern void isr0();
extern void isr1();
extern void isr2();
extern void isr3();
extern void isr4();
extern void isr5();
extern void isr6();
extern void isr7();
extern void isr8();
extern void isr9();
extern void isr10();
extern void isr11();
extern void isr12();
extern void isr13();
extern void isr14();
extern void isr15();
extern void isr16();
extern void isr17();
extern void isr18();
extern void isr19();
extern void isr20();
extern void isr21();
extern void isr28();
extern void isr29();
extern void isr30();
extern void isr31();
extern void undefined_interrupt();

void pic_interrupt_liar();

void (*isr_handlers[32])() =
{
    isr0,
    isr1,
    isr2,
    isr3,
    isr4,
    isr5,
    isr6,
    isr7,
    isr8,
    isr9,
    isr10,
    isr11,
    isr12,
    isr13,
    isr14,
    isr15,
    isr16,
    isr17,
    isr18,
    isr19,
    isr20,
    isr21,
    undefined_interrupt,
    undefined_interrupt,
    undefined_interrupt,
    undefined_interrupt,
    undefined_interrupt,
    undefined_interrupt,
    isr28,
    isr29,
    isr30,
    isr31,
};


struct idt_entry_t idt[256];
struct idt_ptr_t idtp;



void idt_set_gate(uint8_t num, uint32_t handler)
{
    idt[num].offset_low = handler & 0xFFFF;
    idt[num].selector = 0x08;     
    idt[num].zero = 0;
    idt[num].type_attr = 0x8E;     
    idt[num].offset_high = (handler >> 16) & 0xFFFF;
}

void idt_init()
{
    idtp.limit = sizeof(struct idt_entry_t) * 256 - 1;
    idtp.base = (uint32_t)&idt;

    for (int i = 0; i < 32; i++)
    {
        idt_set_gate(i, (uint32_t)isr_handlers[i]);
    }

    for (int i = 32; i < 256; i++)
    {
        idt_set_gate(i, (uint32_t)undefined_interrupt);
    }

    idt_load(&idtp);
    char address[32] = "No address";
    put_str("IDT Loaded at address: ");
    uint32_to_str(idtp.base, address);
    put_str(address);
    put_char('\n');
}

void idt_load(struct idt_ptr_t* idtptr)
{
    __asm__ volatile (
        "lidtl (%0)"
        :
        : "r"(idtptr)
        : "memory"
    );
}

void handle_page_fault(struct RegisterState* cpuState)
{
    char buffer[32];
    uint32_t fault_address = 0;
    asm volatile("mov %%cr2, %0" : "=r"(fault_address));
    uint32_to_str(fault_address, buffer);
    put_str("Fault Address: ");
    put_str(buffer);
    put_char('\n');
    put_str("Error code: ");
    uint32_to_str(cpuState->error_code, buffer);
    put_str(buffer);
    put_char('\n');
}

void general_fault_handler(struct RegisterState* cpuState)
{  
    char buffer[32];
    put_str("EIP: ");
    put_str(uint32_to_str(cpuState->eip, buffer));
    put_str(" Error code: ");
    put_str(uint32_to_str(cpuState->error_code, buffer));
    put_char('\n');
}


void exception_handler(struct RegisterState* cpuState){
    char interrupt[32] = "No Interrupt Found";
    char prefix[] = "ISR Called. Interrupt Number: ";
    put_str(prefix);
    uint32_to_str(cpuState->interrupt, interrupt);
    put_str(interrupt);
    put_char('\n');

    switch(cpuState->interrupt)
    {
        case 14:
            handle_page_fault(cpuState);
            break;
        case 13:
            general_fault_handler(cpuState);
            break;
    }

    for (;;)
    {
        __asm__ volatile ("hlt");
    }
}

void undefined_handler_handler()
{
    char message[] = "No handler found for interrupt\n";

    put_str(message);
    for (;;)
    {
        __asm__ volatile ("hlt");
    }
}