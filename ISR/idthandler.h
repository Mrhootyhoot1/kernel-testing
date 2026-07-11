#include <stdint.h>

struct idt_entry_t {
    uint16_t offset_low;
    uint16_t selector;
    uint8_t zero;
    uint8_t type_attr;
    uint16_t offset_high;
} __attribute__((packed));

struct idt_ptr_t {
    uint16_t limit;           
    uint32_t base;            
} __attribute__((packed));

struct RegisterState
{
    uint32_t gs;
    uint32_t fs;
    uint32_t es;
    uint32_t ds;
    uint32_t edi;
    uint32_t esi;
    uint32_t ebp;
    uint32_t esp;   // original ESP before pushad
    uint32_t ebx;
    uint32_t edx;
    uint32_t ecx;
    uint32_t eax;

    uint32_t interrupt;
    uint32_t error_code;
    uint32_t eip;
    uint32_t cs;
    uint32_t eflags;
};


void idt_set_gate(uint8_t num, uint32_t handler);
void idt_init();
void idt_load(struct idt_ptr_t* idtptr);
