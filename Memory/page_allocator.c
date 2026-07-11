#include "paging.h"
#include "../strlib.h"
#include <stdint.h>
#define PAGE_SIZE 4096
#define MAX_MEMORY (64 * 1024 * 1024)
#define FRAME_COUNT (MAX_MEMORY/PAGE_SIZE)
#include <stddef.h>

uint8_t frame_map[FRAME_COUNT];
// bit 0 is free/allocated. Bits 1 & 2 are the owner

void page_allocator_init()
{
    for (uint32_t i = 0; i < FRAME_COUNT; i++)
    {
        frame_map[i] = 0;
    }
}

void* allocate_frame()
{
    void* address = NULL;
    for (int j = 0; j < FRAME_COUNT; j++)
    {
        if(!(frame_map[j] & 0x01))
        {
            frame_map[j] = frame_map[j] | 0x01;
            address = (void*)(j * PAGE_SIZE);
            set_frame_owner(address, OWNER_KERNEL);
            break;
        }
    }
    return address;
}

void set_frame_used(void* frame)
{
    int index = (uintptr_t)frame/PAGE_SIZE;
    frame_map[index] = frame_map[index] | 0x01;
}

void mark_kernel_used(uint32_t kernel_start, uint32_t kernel_end)
{
    char buffer[64];
    buffer[63] = '\0';
    uint32_to_str(kernel_start, buffer);
    put_str(strcat(buffer, " - Kernel Start\n"));
    uint32_to_str(kernel_end, buffer);
    put_str(strcat(buffer, " - Kernel End\n"));


    uintptr_t firstframe = ((uintptr_t)kernel_start - ((uintptr_t)kernel_start % PAGE_SIZE)); 
    uintptr_t lastframe = ((uintptr_t)kernel_end - ((uintptr_t)kernel_end % PAGE_SIZE));       
    for (uintptr_t i = firstframe; i <= lastframe; i += PAGE_SIZE)
    {
        set_frame_used((void*)i);
        set_frame_owner((void*)i, OWNER_KERNEL);
    }
}

void free_frame(void* frame)
{
    int index = (uintptr_t)frame/PAGE_SIZE;
    frame_map[index] = frame_map[index] ^ 0x01;
    set_frame_owner(frame, OWNER_FREE);
}

void set_frame_owner(void* frame, enum FrameOwner owner)
{
    int index = (uintptr_t)frame/PAGE_SIZE;
    frame_map[index] = (frame_map[index] & 0xF9) | (owner << 1);
}

enum FrameOwner get_frame_owner(void* frame)
{
    int index = (uintptr_t)frame/PAGE_SIZE;
    return (frame_map[index] >> 1) & 0x03;
}


void list_frames()
{
    for(int i = 0; i < FRAME_COUNT; i++)
    {
        if ((frame_map[i] & 0xF9) & 0x01)
        {
            put_str("Frame Allocated: ");
            char number[32] = "N/A";
            uint32_to_str((uint32_t)i, number);
            put_str(number);
            put_char('\n');
        }
    }
}