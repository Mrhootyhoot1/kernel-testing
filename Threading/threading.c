#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>
#include "../Memory/paging.h"
#include "../strlib.h"
#include "threading.h"
#include "Scheduler/scheduler.h"
#define DEFAULT_STACK_SIZE (4096*2)

uint32_t currentId = 0;

struct Thread* FirstThread;
struct Thread* LastThread;

struct Thread* create_kernel_thread();

void kernel_thread_entry();

void init_threading()
{
    //this needs to be the kernel thread
    FirstThread = create_thread(kernel_thread_entry);    
    init_scheduler();
}


struct Thread* create_thread(void (*entry)())
{
    struct Thread* thread = (struct Thread*)kmalloc(sizeof(struct Thread));
    thread->stack_size = DEFAULT_STACK_SIZE;
    thread->stack_start = (uint8_t*)(((uintptr_t)kmalloc(DEFAULT_STACK_SIZE) + DEFAULT_STACK_SIZE) & ~((uintptr_t)0xF));   
    if (thread->stack_start == NULL)
    {
        return NULL;
    }
    char buffer[32];
    struct CPUContext* context = (struct CPUContext *)((uintptr_t)thread->stack_start - sizeof(struct CPUContext));
    thread->esp = (uint32_t)context;
    thread->state = THREAD_READY;
    thread->id = currentId;
    currentId += 1;
    thread->next = NULL;

    uintptr_t raw = (uintptr_t)kmalloc(sizeof(struct FXSave) + 15);

    if (raw == 0)
    {
        return NULL;
    }

    thread->fpu_state = (struct FXSave*)((raw + 15) & ~(uintptr_t)15);

    asm volatile(
    "fninit\n"
    "fxsave %0\n"
    : "=m"(thread->fpu_state)
    );

    context->edi = 0x0;
    context->esi = 0x0;
    context->ebp = 0x0;
    context->esp_dummy = (uint32_t)thread->stack_start;
    context->ebx = 0x0;
    context->ecx = 0x0;
    context->eax = 0x0;

    context->gs = 0x10;   
    context->fs = 0x10;
    context->es = 0x10;
    context->ds = 0x10;

    context->eip = (uint32_t)entry;
    context->cs = 0x08;       
    context->eflags = 0x202;  
    
    thread->previous = LastThread;
    LastThread->next = thread;
    LastThread = thread;
    return thread;
}

void debug_thread(struct Thread* thread)
{
    char buffer[64];
    clear();
    put_str("THREAD DEBUG:\n");
    put_str(strcat(uint32_to_str(thread->id, buffer), " - Thread ID\n"));
   
    put_str(strcat(uint32_to_hex(thread->state, buffer), " - Thread state\n"));
    put_str(strcat(uint32_to_hex(thread->stack_size, buffer), " - Thread Stack Size\n"));
    put_str(strcat(uint32_to_hex((uint32_t)thread->stack_start, buffer), " - Thread Stack start\n"));
    put_str(strcat(uint32_to_hex(thread->esp, buffer), " - Thread esp\n"));

    struct CPUContext* context = (struct CPUContext*)thread->esp;
    put_str(strcat(uint32_to_hex(context->edi, buffer), " - Thread edi\n"));
    put_str(strcat(uint32_to_hex(context->esi, buffer), " - Thread esi\n"));
    put_str(strcat(uint32_to_hex(context->esp_dummy, buffer), " - Thread esp_dummy\n"));
    put_str(strcat(uint32_to_hex(context->ebx, buffer), " - Thread ebx\n"));
    put_str(strcat(uint32_to_hex(context->edx, buffer), " - Thread edx\n"));
    put_str(strcat(uint32_to_hex(context->ecx, buffer), " - Thread ecx\n"));
    put_str(strcat(uint32_to_hex(context->eax, buffer), " - Thread eax\n"));

    //segment registers
    put_str(strcat(uint32_to_hex(context->gs, buffer), " - Thread gs\n"));
    put_str(strcat(uint32_to_hex(context->fs, buffer), " - Thread fs\n"));
    put_str(strcat(uint32_to_hex(context->es, buffer), " - Thread es\n"));
    put_str(strcat(uint32_to_hex(context->ds, buffer), " - Thread ds\n"));

    //interrupt stuff
    put_str(strcat(uint32_to_hex(context->eip, buffer), " - Thread eip\n"));
    put_str(strcat(uint32_to_hex(context->cs, buffer), " - Thread cs\n"));
    put_str(strcat(uint32_to_hex(context->eflags, buffer), " - Thread eflags\n"));
}

void debug_context(struct CPUContext* context)
{
    char buffer[64];
    clear();
    put_str("CPU CONTEXT DEBUG:\n");
   
    put_str(strcat(uint32_to_hex(context->edi, buffer), " - ctx edi\n"));
    put_str(strcat(uint32_to_hex(context->esi, buffer), " - ctx esi\n"));
    put_str(strcat(uint32_to_hex(context->esp_dummy, buffer), " - ctx esp_dummy\n"));
    put_str(strcat(uint32_to_hex(context->ebx, buffer), " - ctx ebx\n"));
    put_str(strcat(uint32_to_hex(context->edx, buffer), " - ctx edx\n"));
    put_str(strcat(uint32_to_hex(context->ecx, buffer), " - ctx ecx\n"));
    put_str(strcat(uint32_to_hex(context->eax, buffer), " - ctx eax\n"));

    //segment registers
    put_str(strcat(uint32_to_hex(context->gs, buffer), " - ctx gs\n"));
    put_str(strcat(uint32_to_hex(context->fs, buffer), " - ctx fs\n"));
    put_str(strcat(uint32_to_hex(context->es, buffer), " - ctx es\n"));
    put_str(strcat(uint32_to_hex(context->ds, buffer), " - ctx ds\n"));

    //interrupt stuff
    put_str(strcat(uint32_to_hex(context->eip, buffer), " - ctx eip\n"));
    put_str(strcat(uint32_to_hex(context->cs, buffer), " - ctx cs\n"));
    put_str(strcat(uint32_to_hex(context->eflags, buffer), " - ctx eflags\n"));
}


extern void restore_cpu_state(struct CPUContext* state);

struct Thread* get_thread(uint32_t id)
{
    struct Thread* current = FirstThread;
    while (current->next != NULL)
    {
        if (current->id == id)
        {
            return current;
        }
        current = current->next;
    }
    return NULL;
}

void free_thread(struct Thread* thread)
{
    kfree(thread->stack_start);
    kfree(thread);
}

void kill_thread(struct Thread* thread)
{
    thread->state = THREAD_TERMINATED;
    thread->next->previous = thread->previous;
    thread->previous->next = thread->next;
}