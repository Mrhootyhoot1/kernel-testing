#ifndef THREADING_H
#define THREADING_H

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

struct FXSave
{
    uint16_t fcw;          // FPU Control Word
    uint16_t fsw;          // FPU Status Word
    uint8_t  ftw;          // Compressed Tag Word
    uint8_t  reserved1;
    uint16_t fop;          // Last Opcode

    uint32_t fip;          // FPU Instruction Pointer
    uint16_t fcs;          // FPU CS
    uint16_t reserved2;

    uint32_t fdp;          // FPU Data Pointer
    uint16_t fds;          // FPU DS
    uint16_t reserved3;

    uint32_t mxcsr;        // MXCSR Register
    uint32_t mxcsr_mask;   // MXCSR Mask

    // ST/MM registers (8 × 16 bytes)
    uint8_t st_mm[8][16];

    // XMM registers (8 × 16 bytes)
    uint8_t xmm[8][16];
    
    // Reserved
    uint8_t reserved4[224];
}__attribute__((aligned(16)));;
_Static_assert(sizeof(struct FXSave) == 512, "FXSave must be 512 bytes");
_Static_assert(_Alignof(struct FXSave) == 16,"FXSave must be 16-byte aligned");

struct CPUContext
{
    uint32_t edi;
    uint32_t esi;
    uint32_t ebp;
    uint32_t esp_dummy; //why does this even exist? It's ignored by popa.
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
}__attribute__((aligned(16)));;
_Static_assert(_Alignof(struct CPUContext) == 16,"CPU context must be 16-byte aligned");

struct Thread
{
    uint32_t id;
    enum ThreadState state;

    uint8_t* stack_start;
    uint32_t stack_size;
    uint32_t esp;

    struct FXSave* fpu_state;

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
#endif