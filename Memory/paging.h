#include <stdint.h>
#include <stddef.h>

struct BiosMemoryMap
{
    uint64_t base_address;
    uint64_t length;
    uint32_t type;
    uint32_t attributes;
};

enum FrameOwner
{
    OWNER_FREE = 0,
    OWNER_KERNEL = 1,
    OWNER_USER = 2
};

void init_paging(struct BiosMemoryMap* memory_map, uint16_t entry_count);
void page_allocator_init();
void* allocate_frame();
void free_frame(void* frame);
void set_frame_owner(void* frame, enum FrameOwner owner);
enum FrameOwner get_frame_owner(void* frame);
void set_frame_used(void* frame);
void mark_kernel_used(uint32_t kernel_start, uint32_t kernel_end);
void list_frames();
void* kmalloc(size_t size);
void kfree();
void init_memalloc();