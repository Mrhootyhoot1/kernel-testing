#include <stdint.h>
extern uint32_t pit_frequency;
extern volatile uint32_t ticks;
void timer_init(uint32_t frequency);