#include <stdint.h>

void irq_handlers_init();
struct irq_frame
{
    uint32_t gs;
    uint32_t fs;
    uint32_t es;
    uint32_t ds;

    uint32_t edi;
    uint32_t esi;
    uint32_t ebp;
    uint32_t esp_dummy;
    uint32_t ebx;
    uint32_t edx;
    uint32_t ecx;
    uint32_t eax;

    uint32_t irq;
    uint32_t error;

    uint32_t eip;
    uint32_t cs;
    uint32_t eflags;
};