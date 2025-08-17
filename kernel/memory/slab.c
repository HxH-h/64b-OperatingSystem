# include "slab.h"
# include "memory.h"

#define ALIGN_ADDR_UP(address, size) \
    (((uint64_t)(address) + (uint64_t)(size) - 1) & ~((uint64_t)(size) - 1))

# define GET_SLAB_ITEM(address, size) \
    ((uint64_t)(address) & ~((uint64_t)(size) - 1))

// 创建slab
Slab * slab_create(Pool_type type , uint32_t slab_size){
    Slab *slab = (Slab*)page_alloc(type, 1);
    
    slab->slab_size = slab_size;
    
    uint64_t slab_end = (uint64_t)slab + sizeof(Slab);
    
    slab->vaddr_start = (void *)(ALIGN_ADDR_UP(slab_end, slab_size));
    
    slab->vaddr_end = (void *)((uint64_t)slab + PAGE_SIZE);

    slab->total = ((uint64_t)slab->vaddr_end - (uint64_t)slab->vaddr_start) / slab_size;
    slab->total_free = slab->total;

    slab->magic = SLAB_MAGIC;

    list_init(&slab->free_list);

    uint32_t i = 0;
    for (; i < slab->total; i++){
        Node *slab_item = (Node *)((uint64_t)slab->vaddr_start + i * slab_size);
        list_append(&slab->free_list, slab_item);
    }
    
    return slab;
}

void slab_init(Slab_cache* cache , uint32_t slab_size , slab_callback constructor , slab_callback destructor){

    cache->slab_size = slab_size;
    cache->constructor = constructor;
    cache->destructor = destructor;

    list_init(&cache->slab_list);


    // TODO 实现用户态的分配

    Pool_type type = KERNEL_2M;

    Slab *slab = slab_create(type, slab_size);


    list_push(&cache->slab_list, &slab->node);

    cache->total = slab->total;
    cache->total_free = slab->total_free;

}

// 分配slab
void * slab_alloc(Pool_type type , Slab_cache* cache){

    if (list_empty(&cache->slab_list)){
        Slab *slab = slab_create(type, cache->slab_size);
        list_push(&cache->slab_list, &slab->node);
        cache->total += slab->total;
        cache->total_free += slab->total_free;
    }

    // 获取 slab cache 第一个slab
    Node *head = cache->slab_list.head.next;
    Slab *slab = (Slab *)((uint64_t)head & SLAB_MASK);

    Node *item = list_pop(&slab->free_list);
    void *addr = GET_SLAB_ITEM(item, cache->slab_size);

    memset(addr, 0, cache->slab_size);

    cache->total_free--;
    slab->total_free--;

    if(slab->total_free == 0) list_pop(&cache->slab_list);


    return addr;

}

// slab 块释放
void slab_free(Pool_type type , Slab_cache* cache, void* _addr){

    Slab *slab = (Slab *)((uint64_t)_addr & SLAB_MASK);
    Node *node = (Node *)GET_SLAB_ITEM((uint64_t)_addr, slab->slab_size);

    list_push(&slab->free_list, node);

    slab->total_free++;
    cache->total_free++;
    
    if(slab->total_free == 1) list_push(&cache->slab_list, &slab->node);
    else if (slab->total_free == slab->total){
        list_remove(&slab->node);
        cache->total_free -= slab->total;

        bool res = page_free(type, slab, 1);
        if(!res){
            print("free slab fail\n");
        }
          
    }
    
}





