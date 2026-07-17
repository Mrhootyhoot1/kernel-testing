#include <stddef.h>
#include <stdint.h>

enum ThreadState
{
    THREAD_READY,
    THREAD_RUNNING,
    THREAD_BLOCKED,
    THREAD_SLEEPING,
    THREAD_TERMINATED
};

struct CPUContext
{
    uint32_t edi;
    uint32_t esi;
    uint32_t ebp;
    uint32_t esp_dummy; // value saved by pusha, usually ignored
    uint32_t ebx;
    uint32_t edx;
    uint32_t ecx;
    uint32_t eax;

    // Segment registers
    uint32_t gs;
    uint32_t fs;
    uint32_t es;
    uint32_t ds;

    // Saved by interrupt CPU entry
    uint32_t eip;
    uint32_t cs;
    uint32_t eflags;
};

struct Thread
{
    uint32_t id;
    enum ThreadState state;

    uint8_t* stack_start;
    uint32_t stack_size;
    uint32_t esp;

    struct Thread* next;
    struct Thread* previous;
};

extern struct Thread* RunningThread;

struct Thread* create_thread(void (*entry)());
struct Thread* get_thread(uint32_t id);
struct Thread* get_next_thread();
void init_threading();
void debug_thread(struct Thread* thread);
void debug_context(struct CPUContext* context);
void kill_thread(struct Thread* thread);
void free_thread(struct Thread* thread);