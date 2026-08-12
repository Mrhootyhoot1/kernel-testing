#include "paging.h"
#include "../strlib.h"
#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>
#define PAGE_SIZE 4096

struct AllocationList
{
    struct Allocation* first;
    uint32_t allocated_pages;
    uint32_t count;
};

struct AllocationList list;

struct Allocation* currentNode;

struct Allocation
{
    uintptr_t startAddress;
    uintptr_t endAddress;
    uint32_t pageStart;
    uint32_t pageEnd;
    //list stuff
    struct Allocation* nextNode;
};

void init_memalloc()
{
    list.count = 1;
    void* memptr = allocate_frame();
    struct Allocation* alloc = (struct Allocation*)memptr;
    list.first = alloc;
    list.first->startAddress = (uintptr_t)memptr;
    list.first->endAddress = (uintptr_t)memptr + PAGE_SIZE;
    list.first->pageStart = list.first->startAddress / PAGE_SIZE;
    list.first->pageEnd = list.first->pageStart;
    list.first->nextNode = NULL;
    list.allocated_pages = 1;
    currentNode = alloc;
}

void append_node(struct Allocation node)
{
    //put_str("PAGE START, PAGE END: ");
    if ((list.count + 1) * sizeof(struct Allocation) > list.allocated_pages * PAGE_SIZE)
    {
        currentNode->nextNode = (struct Allocation*)allocate_frame();
        list.allocated_pages += 1;
    }
    else
    {
        currentNode->nextNode = (struct Allocation*)((uintptr_t)currentNode + sizeof(struct Allocation));
    }
    currentNode = currentNode->nextNode;
    currentNode->startAddress = node.startAddress;
    currentNode->endAddress = node.endAddress;
    currentNode->pageStart = node.pageStart;
    currentNode->pageEnd = node.pageEnd;
    //char buffer[32];
    //put_str(uint32_to_str(currentNode->pageStart, buffer));
    //put_str(", ");
    //put_str(uint32_to_str(currentNode->pageEnd, buffer));
    //put_char('\n');
    currentNode->nextNode = NULL;
}

void* kmalloc(size_t size)
{
    size = (size + 7) & ~7;
    static uint32_t remaining_page_space = 0; 
    static uintptr_t current_page_address = 0;
    struct Allocation* node = list.first;
    struct Allocation alloc;
    while(node->nextNode != NULL)
    {
        if ((uintptr_t)node->nextNode->startAddress - (uintptr_t)node->endAddress >= size && node->pageStart == node->nextNode->pageStart)
        {
            alloc.startAddress = node->endAddress;
            alloc.endAddress = node->endAddress + size;
            alloc.pageStart = alloc.startAddress/PAGE_SIZE;
            alloc.pageEnd = alloc.pageStart;
            alloc.nextNode = NULL;
            append_node(alloc);
            return (void*)alloc.startAddress;
        }
        node = node->nextNode;
    }
    if(size > remaining_page_space)
    {
        uint32_t allocated_size = PAGE_SIZE;
        current_page_address = (uintptr_t)allocate_frame();
        //char buffer[32];
        //put_str("Page allocated at address: ");
        //put_str(uint32_to_str((uint32_t)current_page_address, buffer));
        put_char('\n');
        while(allocated_size < size)
        {
            void* frame = allocate_frame();
            map_page((uint32_t)(uintptr_t)current_page_address + allocated_size, (uint32_t)frame);
            //put_str("Page allocated at virtual address: ");
            //put_str(uint32_to_str((uint32_t)current_page_address + allocated_size, buffer));
            //put_char('\n');
            allocated_size += PAGE_SIZE;
        }
        alloc.startAddress = current_page_address;
        alloc.endAddress = alloc.startAddress + size;
        alloc.pageStart = alloc.startAddress/PAGE_SIZE;
        alloc.pageEnd = alloc.pageStart + allocated_size/PAGE_SIZE;
        alloc.nextNode = NULL;
        append_node(alloc);
        remaining_page_space = allocated_size - size;
        return (void*)alloc.startAddress;
    }
    alloc.startAddress = current_page_address + (PAGE_SIZE - remaining_page_space);
    alloc.endAddress = alloc.startAddress + size;
    alloc.pageStart = alloc.startAddress/PAGE_SIZE;
    alloc.pageEnd = alloc.pageStart;
    remaining_page_space -= size;
    append_node(alloc);
    return (void*)alloc.startAddress;
}

struct Allocation* get_allocation(void* memory)
{
    struct Allocation* node = list.first;
    while (node->nextNode != NULL)
    {
        if (node->nextNode->startAddress == (uintptr_t)memory)
        {
            return node->nextNode;
            node->nextNode = node->nextNode->nextNode;
        }

        node = node->nextNode;
    }
    return NULL;
}

void* krealloc(void* memory, size_t size)
{
    //this will end up being the node before the allocation
    struct Allocation* node = list.first;
    struct Allocation* allocation = NULL;
    while (node->nextNode != NULL)
    {
        if (node->nextNode->startAddress == (uintptr_t)memory)
        {
            allocation = node->nextNode;
        }

        node = node->nextNode;
    }

    if (allocation == NULL)
    {
        return NULL;
    }

    if (allocation->endAddress - allocation->startAddress == size)
    {
        return memory;
    }

    if (allocation->nextNode->startAddress - allocation->endAddress >= size)
    {
        allocation->endAddress = allocation->startAddress + size;
        return memory;
    }

    void* new_allocation = kmalloc(size);
    if (new_allocation == NULL)
    {
        return NULL;
    }
    memmov(new_allocation, memory, size);
    kfree(memory);
    return new_allocation;
}

//very shit
void kfree(void* memory)
{
    if (memory == NULL)
    {
        return;
    }

    uint32_t page_start = (uint32_t)((uintptr_t)memory/PAGE_SIZE);

    struct Allocation* node = list.first;
    struct Allocation* allocation = NULL;

    bool other_allocations = false;
    while (node->nextNode != NULL)
    {
        if(node->nextNode->pageStart == page_start || node->nextNode->pageEnd == page_start)
        {
            other_allocations = true;
        }
        if (node->nextNode->startAddress == (uintptr_t)memory)
        {
            allocation = node->nextNode;
            node->nextNode = node->nextNode->nextNode;
        }

        node = node->nextNode;
    }

    if(allocation == NULL)
    {
        return;
    }

    if(!other_allocations)
    {
        free_frame((void*)(page_start*PAGE_SIZE));
    }

    for (uint32_t i = allocation->pageStart + 1; i < (allocation->endAddress - allocation->startAddress)/PAGE_SIZE + allocation->pageStart; i++)
    {
        uint32_t addr = page_start*PAGE_SIZE + i * PAGE_SIZE;
        free_frame((void*)addr);
    }
}