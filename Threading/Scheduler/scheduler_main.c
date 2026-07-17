#include <stddef.h>
#include <stdint.h>
#include "../../Memory/paging.h"
#include "../../strlib.h"
#include "../threading.h"
#include "../../PIC/picmapper.h"
#include "../../PIC/timing.h"
#define DEFAULT_STACK_SIZE (4096*2)
#define CONTEXT_SWITCH_FREQUENCY 100

struct Thread* RunningThread;

void init_scheduler()
{
    RunningThread = get_thread(0);
}

struct CPUContext* timer_irq_handler(struct CPUContext* current_context)
{
    ticks++;
    if (ticks % (pit_frequency / CONTEXT_SWITCH_FREQUENCY) != 0)
    {
        ack_interrupt();
        return current_context;
    }
    if(RunningThread->state != THREAD_TERMINATED)
    {
        RunningThread->esp = (uint32_t)current_context;
        RunningThread->state = THREAD_READY;
    }
    else
    {
        free_thread(RunningThread);
    }
    ack_interrupt();
    struct Thread* next_thread = get_next_thread();
    next_thread->state = THREAD_RUNNING;
    RunningThread = next_thread;
    //debug_thread(next_thread);
    return (struct CPUContext*)next_thread->esp;
}

struct Thread* get_next_thread()
{
    struct Thread* current;
    current = RunningThread;

    if(current->next == NULL)
    {
        current = get_thread(0);
        return current;
    }
    
    current = current->next;

    while(current->state != THREAD_READY)
    {
        if(current->next == NULL)
        {
            current = get_thread(0);
            return current;
        }
        current = current->next;
    }
    return current;
}
