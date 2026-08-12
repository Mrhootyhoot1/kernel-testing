#include <stdint.h>
#include <stddef.h>
#include "../../strlib.h"
#include "../../PIC/input.h"
#include "../../ISR/idthandler.h"
#include "../../Threading/Scheduler/scheduler.h"
#include "../../Threading/threading.h"

struct CPUContext* yield_cpu(struct CPUContext* currentContext);

struct CPUContext* (*call_numbers[208])() =
{
    [0x01] = yield_cpu,
};

void syscall(uint32_t num)
{
        asm volatile (
            "mov %0, %%eax\n"
            "int $0x80"
            :
            : "r"(num)
            : "eax"
        );
}

struct CPUContext* syscall_handler(struct CPUContext* current_context)
{
    uint32_t call_number;

    __asm__ volatile (
    "movl %%eax, %0"
    : "=m"(call_number)
    );

    if(call_numbers[call_number] == NULL)
    {
        return current_context;
    }

    return call_numbers[call_number]();
}

void yield()
{
    syscall(0x01);
}

struct CPUContext* yield_cpu(struct CPUContext* current_context)
{
    struct Thread* next_thread = get_new_thread(current_context);
    return (struct CPUContext*)next_thread->esp;
}