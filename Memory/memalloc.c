#include "paging.h"
#include "../strlib.h"
#include <stdint.h>
#include <stddef.h>
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
    uint32_t page;
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
    list.first->page = list.first->startAddress / PAGE_SIZE;
    list.first->nextNode = NULL;
    list.allocated_pages = 1;
    currentNode = alloc;
}

void append_node(struct Allocation node)
{
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
    currentNode->page = node.page;
    currentNode->nextNode = NULL;
}

void* kmalloc(size_t size)
{
    if(size > PAGE_SIZE)
    {
        return NULL;
    }
    static uint32_t remaining_page_space = 0; 
    static uintptr_t current_page_address = 0;
    struct Allocation* node = list.first;
    struct Allocation alloc;
    while(node->nextNode != NULL)
    {
        if ((uintptr_t)node->nextNode->startAddress - (uintptr_t)node->endAddress >= size && node->page == node->nextNode->page)
        {
            alloc.startAddress = node->endAddress;
            alloc.endAddress = node->endAddress + size;
            alloc.page = alloc.startAddress/PAGE_SIZE;
            alloc.nextNode = NULL;
            append_node(alloc);
            return (void*)alloc.startAddress;
        }
        node = node->nextNode;
    }
    if(size > remaining_page_space)
    {
        current_page_address = (uintptr_t)allocate_frame();
        alloc.startAddress = current_page_address;
        alloc.endAddress = alloc.startAddress + size;
        alloc.page = alloc.startAddress/PAGE_SIZE;
        alloc.nextNode = NULL;
        append_node(alloc);
        remaining_page_space = PAGE_SIZE - size;
        return (void*)alloc.startAddress;
    }
    alloc.startAddress = current_page_address + (PAGE_SIZE - remaining_page_space);
    alloc.endAddress = alloc.startAddress + size;
    alloc.page = alloc.startAddress / PAGE_SIZE;
    remaining_page_space -= size;
    append_node(alloc);
    return (void*)alloc.startAddress;
} 

void kfree(void* memory)
{
    if (memory == NULL)
    {
        return;
    }

    struct Allocation* node = list.first;
    while (node->nextNode != NULL)
    {
        if (node->nextNode->startAddress == (uintptr_t)memory)
        {
            node->nextNode = node->nextNode->nextNode;
            return;
        }

        node = node->nextNode;
    }
}