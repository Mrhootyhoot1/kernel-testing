#include <stdint.h>
#include <stddef.h>
#include "../strlib.h"
#include "../Memory/paging.h"
#include "IRQ/irqhandlers.h"
#include "picmapper.h"
#define PIT_COMMAND_PORT 0x43
#define PIT_CHANNEL_0 0x40
// low byte, high byte, square wave mode, binary counting
#define PIT_CONFIGURE_COMMAND 0b00111110
#define CLOCK_SPEED 1193182

volatile uint32_t ticks = 0;

uint32_t pit_frequency = 0;

void timer_init(uint16_t frequency)
{
    uint16_t divisor = CLOCK_SPEED/frequency;

    pit_frequency = frequency;

    if(divisor == 1 || divisor == 0)
    {
        put_str("A divisor of 1 or 0 may not be used in square wave mode on the PIT\n");
        put_str("The PIT has defaulted to 100 hz\n");
        divisor = CLOCK_SPEED/100;
        pit_frequency = 100;
    }

    outb(PIT_COMMAND_PORT, PIT_CONFIGURE_COMMAND); // configure the PIT
    outb(PIT_CHANNEL_0, divisor & 0xFF);           // Write the low byte
    outb(PIT_CHANNEL_0, divisor >> 8);             // Write the high byte
}

