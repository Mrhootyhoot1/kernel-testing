#include <stdint.h>
#include "paging.h"
#include <stddef.h>
#include "../strlib.h"
#define PAGE_TABLE_SIZE (4096*1024)

extern void load_page_directory(uint32_t* page_directory);
extern void enable_paging();
void create_page_table();

uint32_t page_directory[1024] __attribute__((aligned(4096)));
uint32_t page_tables[1024][1024] __attribute__((aligned(4096)));


uint32_t get_total_memory(struct BiosMemoryMap* memory_map, uint16_t entry_count)
{
    uint32_t total_memory = 0;
    for(int i = 0; i < (int)entry_count; i++)
    {
        if(!(memory_map[i].type == 1)) 
        {
            continue;
        }
        total_memory += memory_map[i].length;
    }
    return total_memory;
}


void init_paging(struct BiosMemoryMap* memory_map, uint16_t entry_count)
{
    for (int i = 0; i < 1024; i++)
    {
        page_directory[i] = 0;
    }

    uint32_t total_memory = get_total_memory(memory_map, entry_count);

    char buffer[32];
    uint32_to_str(total_memory/1024, buffer);
    put_str("Total Memory (in kib): ");
    put_str(buffer);
    put_char('\n');

    for (uint32_t i = 0; i < total_memory / PAGE_TABLE_SIZE; i++)
    {
        create_page_table();
        //just assume 64 mb of memory
    }


    load_page_directory((uint32_t*)&page_directory);
    enable_paging();
    page_allocator_init();
}


void create_page_table()
{
    static uint32_t pages = 0;
    uint32_t offset = pages * 0x400000;
    uint32_t i;

    for (i = 0; i < 1024; i++)
    {
        page_tables[pages][i] = (offset + i * 0x1000) | 3;
    }

    page_directory[pages] = ((uint32_t)page_tables[pages]) | 3;
    pages++;
}
