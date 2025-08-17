# pragma once

# include "../lib/stdtype.h"
# include "../lib/linkedlist.h"
# include "memory.h"
# include "../console/console.h"

# define SLAB_MAGIC 0x4858480426
# define SLAB_MASK (0xffffffffffe00000)

typedef void* (* slab_callback)(void* vaddr ,uint64_t arg); 

typedef struct{
    Node node;  // 挂载到slab_cache

    uint32_t slab_size;
    uint32_t total;    // 块数
    uint32_t total_free;

    LinkedList free_list; // 管理空闲区

    void* vaddr_start;
    void* vaddr_end;

    uint64_t magic;

} Slab;

typedef struct{
    uint32_t slab_size;

    uint32_t total;    // 块数
    uint32_t total_free;

    LinkedList slab_list;

    slab_callback constructor;
    slab_callback destructor;

} Slab_cache;

void slab_init(Slab_cache* cache , uint32_t slab_size , slab_callback constructor , slab_callback destructor);
void * slab_alloc(Pool_type type , Slab_cache* cache);
void slab_free(Pool_type type , Slab_cache* cache, void* _addr);

