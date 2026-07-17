#include <stdint.h>
#include "strlib.h"
#include "ISR/idthandler.h"
#include "Memory/paging.h"
#include "PIC/picmapper.h"
#include "PIC/IRQ/irqhandlers.h"
#include "PIC/timing.h"
#include "Threading/Scheduler/scheduler.h"
#include "Threading/threading.h"

extern char _kernel_start;
extern char _kernel_end;

int check_a20()
{
    unsigned char* low = (unsigned char *)0x000000;
    unsigned char* high = (unsigned char *)0x100000;
    char originalLow = *low;
    char originalHigh = *high;

    *low = 0xAA;
    *high = 0x55;

    if (*low == 0x55)
    {
        *low = originalLow;
        *high = originalHigh;
        return 0;
    }

    *low = originalLow;
    *high = originalHigh;
    return 1;
}



void kernel_main(struct BiosMemoryMap* memory_map, uint16_t entry_count)
{
    put_str("Kernel Loaded (16 sectors)\n");

    idt_init();

    if(check_a20())
    {
        put_str("A20 is enabled\n");
    }
    else
    {
        put_str("A20 is not enabled\n");
    }

    irq_handlers_init();

    timer_init(100);

    init_paging(memory_map, entry_count);

    uintptr_t kernel_start = (uintptr_t)&_kernel_start;
    uintptr_t kernel_end   = (uintptr_t)&_kernel_end;

    put_str("Reserving kernel memory\n");
    mark_kernel_used((uint32_t)&kernel_start, (uint32_t)&kernel_end);

    init_memalloc();

    put_str("Initialized paging\n");

    char str[] = "On the heap\n";
    char* buffer = kmalloc(16000);

    strcpy(buffer, str);
    put_str(buffer);
    kfree(buffer);

    init_threading();

    __asm__ volatile ("sti");
    put_str("Interrupts restored \n)");

    //unmask all interrupts
    outb(0x21, 0x00);
    outb(0xA1, 0x00);

    put_str("Unmasked interrupts\n");
    for (;;)
    {   
       __asm__ volatile ("hlt");
    }
}

//the kernel is now on a thread with id 0. The stack has been 
void kernel_thread_entry()
{
    put_str("Kernel new thread\n");
    for (;;)
    {   
       __asm__ volatile ("hlt");
    }
}
