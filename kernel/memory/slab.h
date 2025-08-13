# pragma once

# include "../lib/stdtype.h"
# include "../lib/linkedlist.h"


typedef void* (* slab_callback)(void* vaddr ,uint64_t arg); 

typedef struct{
    Node node;  // 挂载到slab_cache

    uint32_t total;    // 块数
    uint32_t total_free;

    LinkedList free_list; // 管理空闲区

    void* vaddr_start;
    void* vaddr_end;

} Slab;

typedef struct{
    uint32_t slab_size;

    uint32_t total;    // 块数
    uint32_t total_free;

    LinkedList slab_list;

    slab_callback constructor;
    slab_callback destructor;

} Slab_cache;




