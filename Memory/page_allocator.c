#include "paging.h"
#include "../strlib.h"
#include <stdint.h>
#define PAGE_SIZE 4096
#define MAX_MEMORY (64 * 1024 * 1024)
#define FRAME_COUNT (MAX_MEMORY/PAGE_SIZE)
#define HEAP_START 0x00100000
#define FRAME_OFFSET (HEAP_START/PAGE_SIZE)
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
    for (int j = FRAME_OFFSET; j < FRAME_COUNT; j++)
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
    uintptr_t firstframe = ((uintptr_t)kernel_start - ((uintptr_t)kernel_start % PAGE_SIZE)); 
    uintptr_t lastframe = ((uintptr_t)kernel_end - ((uintptr_t)kernel_end % PAGE_SIZE));       
    for (uintptr_t i = firstframe; i <= lastframe; i += PAGE_SIZE)
    {
        set_frame_used((void*)i);
        set_frame_owner((void*)i, OWNER_KERNEL);
    }
}

uint32_t get_physical_address(uint32_t virtual)
{
    uint32_t pd_index = virtual >> 22;
    uint32_t pt_index = (virtual >> 12) & 0x3FF;
    uint32_t offset = virtual & 0xFFF;

    uint32_t entry = page_tables[pd_index][pt_index];

    if (!(entry & 0x1))
    {
        return 0; // not mapped
    }

    uint32_t physical_page = entry & 0xFFFFF000;

    return physical_page + offset;
}

void free_frame(void* frame)
{
    //put_str("Freeing frame\n");
    frame = (void*)get_physical_address((uint32_t)frame);
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